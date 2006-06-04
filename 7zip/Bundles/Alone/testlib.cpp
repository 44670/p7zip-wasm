
#include "StdAfx.h"

// #include "../../../Common/MyInitGuid.h"
#include "../../../Common/ComTry.h"

#include "../../ICoder.h"

#include "../../Common/FileStreams.h"
#include "Windows/PropVariant.h"
#include "../../Archive/IArchive.h"

#include "Common/MyCom.h"
#include "7zip/PropID.h"

#include "Common/String.h"

/* #include "7zHandler.h" */

#include <vector>

#include "../../Common/FileStreams.cpp"

const UInt64 kMaxCheckStartPosition = 
#ifdef _WIN32
1 << 20;
#else
1 << 22;
#endif

UInt64 ConvertPropVariantToUInt64(const PROPVARIANT &propVariant)
{
  switch (propVariant.vt)
  {
    case VT_UI1:
      return propVariant.bVal;
    case VT_UI2:
      return propVariant.uiVal;
    case VT_UI4:
      return propVariant.ulVal;
    case VT_UI8:
      return (UInt64)propVariant.uhVal.QuadPart;
    default:
      #ifndef _WIN32_WCE
      throw 151199;
      #else
      return 0;
      #endif
  }
}


STDAPI CreateObject(
    const GUID *classID, 
    const GUID *interfaceID, 
    void **outObject);

STDAPI GetHandlerProperty(PROPID propID, PROPVARIANT *value);

class CExtractCallback : public IArchiveExtractCallback , public CMyUnknownImp
{
CMyComPtr<IInArchive> _archiveHandler;
public:
  // MY_UNKNOWN_IMP ;
  virtual LONG QueryInterface (const GUID & iid, void **outObject) {
   return ((LONG)0x80004002L);
  }
  virtual ULONG AddRef() { return ++__m_RefCount; }
  virtual ULONG Release() { 
     if (--__m_RefCount != 0) return __m_RefCount;
     delete this;
     return 0;
   }
  //

  CExtractCallback(IInArchive *archive)
  {
    _archiveHandler = archive;
  }

  STDMETHOD(GetStream)(UInt32 index, ISequentialOutStream **outStream, 
      Int32 askExtractMode)
   {
	NWindows::NCOM::CPropVariant propVariant;
	HRESULT res = _archiveHandler->GetProperty(index, kpidPath, &propVariant);
	if (res != S_OK) return res;

	if (propVariant.vt == VT_BSTR)
	{
		UString fullPath;
		fullPath = propVariant.bstrVal;
		*outStream = 0;

		res = _archiveHandler->GetProperty(index, kpidSize, &propVariant);
		if (res == S_OK)
		{
			UInt64 newFileSize = ConvertPropVariantToUInt64(propVariant);
			printf("GetStream(%d)='%ls' (%ld)\n",index,&fullPath[0],(long)newFileSize);
		} else {
			printf("GetStream(%d)='%ls' (no size)\n",index,&fullPath[0]);
		}
		return S_OK;
	}
	printf("GetStream : E_FAIL !\n");
	return E_FAIL;
   }
  // GetStream OUT: S_OK - OK, S_FALSE - skeep this file
  STDMETHOD(PrepareOperation)(Int32 askExtractMode)
   {
		printf("PrepareOperation(%d)\n",askExtractMode);
		return S_OK;
   }
  STDMETHOD(SetOperationResult)(Int32 resultEOperationResult)
   {
		printf("SetOperationResult(%d)\n",resultEOperationResult);
		return S_OK;
   }
  STDMETHOD(SetTotal)(UInt64 total)
   {
		printf("SetTotal(%ld)\n",(long)total);
		return S_OK;
   }
  STDMETHOD(SetCompleted)(const UInt64 *completeValue)
   {
		printf("SetCompleted(%ld)\n",(long)(*completeValue));
		return S_OK;
   }

};


static void showProperty(const char *name,PROPID propID)
{
	NWindows::NCOM::CPropVariant prop;

	if (GetHandlerProperty(propID, &prop) == S_OK)
	{
		if (prop.vt == VT_BSTR)
		{
			printf("%s:'%ls'\n",name,(const wchar_t *)prop.bstrVal);
		}
		prop.Clear();
	}
}

static void showBoolProperty(const char *name,PROPID propID)
{
	NWindows::NCOM::CPropVariant prop;

	if (GetHandlerProperty(propID, &prop) == S_OK)
	{
		if (prop.vt == VT_BOOL)
		{
			printf("%s: %s\n",name,(prop.boolVal == VARIANT_FALSE ? "False" : "True"));
		}
		prop.Clear();
	}
}

int main(int argc,char *argv[])
{

	if (argc != 2) {
		printf("usage : %s archive.7z\n",argv[0]);
		exit(EXIT_FAILURE);
	}
	showProperty("NArchive::kName",NArchive::kName);
	showProperty("NArchive::kExtension",NArchive::kExtension);
	// ? showProperty("NArchive::kAddExtension",NArchive::kAddExtension);
	showBoolProperty("NArchive::kUpdate",NArchive::kUpdate);
	showBoolProperty("NArchive::kKeepName",NArchive::kKeepName);
	// BSTR showProperty("NArchive::kStartSignature",NArchive::kStartSignature);

	// GUID showProperty("NArchive::kClassID",NArchive::kClassID);
	NWindows::NCOM::CPropVariant prop;
	CLSID ClassID;
	CMyComPtr<IInArchive> archive;

	if (GetHandlerProperty(NArchive::kClassID, &prop) == S_OK)
	{
		if (prop.vt == VT_BSTR)
		{
			ClassID = *(const GUID *)prop.bstrVal;
		}
		prop.Clear();
	}
	

	HRESULT result = CreateObject(&ClassID, &IID_IInArchive,(void **)&archive);
	if (result == S_OK)
	{
		const char *filePath=argv[1];
		printf("ArchiveIn : OK\n");

		CInFileStream *fileSpec = new CInFileStream;
		CMyComPtr<IInStream> file = fileSpec;
		if (!fileSpec->Open(filePath))
		{
			printf("error : '%s' (%d)\n",filePath,GetLastError());
		} else {
			printf("Opening : '%s'\n",filePath);

			result = archive->Open(file, &kMaxCheckStartPosition, 0 /* openArchiveCallback*/ );

			if (result == S_OK)
			{
				UInt32 numItems;
				result = archive->GetNumberOfItems(&numItems);
				if (result == S_OK)
				{
					printf("GetNumberOfItems : %d\n",(int)numItems);
					std::vector<UInt32> tabIndex;
					for(UInt32 index = 0; index < numItems ; index++)
					{
						result = archive->GetProperty(index, kpidPath, &prop);
						if (prop.vt == VT_BSTR)
						{
							printf("%d :'%ls'\n",index,(const wchar_t *)prop.bstrVal);
						}
						prop.Clear();

						result = archive->GetProperty(index, kpidIsFolder, &prop);
						if (prop.vt == VT_BOOL)
						{
							printf("\tIsFolder %s\n",(prop.boolVal == VARIANT_FALSE ? "False" : "True"));
							if (prop.boolVal == VARIANT_FALSE) tabIndex.push_back(index);
						}
						prop.Clear();
					}
					if (tabIndex.size() >= 1)
					{
						printf("Testing %d files\n",(int)tabIndex.size());
						CExtractCallback * extractCallbackSpec = new CExtractCallback(archive);
						// 1 : test mode
						// 0 : extract
						result = archive->Extract(&tabIndex[0], (UInt32)tabIndex.size(),1,extractCallbackSpec);
						if (result == S_OK) printf(" Extract : OK\n");
						else                printf(" Extract : BAD\n");
						// extractCallbackSpec is already deleted by archive->Extract
					}
	
				} else {
					printf("ERROR in GetNumberOfItems\n");
				}
			} else {
				printf("ERROR in archive->Open\n");
			}
		}


	}


	return 0;
}

/*
UI/Common/Extract.cpp
DecompressArchive
*/


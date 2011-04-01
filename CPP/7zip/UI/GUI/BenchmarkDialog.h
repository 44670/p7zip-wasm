// BenchmarkDialog.h

#ifndef __BENCHMARK_DIALOG_H
#define __BENCHMARK_DIALOG_H

#include "Windows/Synchronization.h"
#include "Windows/Control/ComboBox.h"

#include "../Common/Bench.h"

#include "../FileManager/DialogSize.h"

#include "BenchmarkDialogRes.h"

struct CBenchInfo2 : public CBenchInfo
{
  void Init()  { GlobalTime = UserTime = 0; }
};

class CProgressSyncInfo
{
public:
  bool Stopped;
  bool Paused;
  bool Changed;
  UInt32 DictionarySize;
  UInt32 NumThreads;
  UInt64 NumPasses;
  // UInt64 NumErrors;
  NWindows::NSynchronization::CManualResetEvent _startEvent;
  NWindows::NSynchronization::CCriticalSection CS;

  CBenchInfo2 CompressingInfoTemp;
  CBenchInfo2 CompressingInfo;
  UInt64 ProcessedSize;

  CBenchInfo2 DecompressingInfoTemp;
  CBenchInfo2 DecompressingInfo;

  CProgressSyncInfo()
  {
    if (_startEvent.Create() != S_OK)
      throw 3986437;
  }
  void Init()
  {
    Changed = false;
    Stopped = false;
    Paused = false;
    CompressingInfoTemp.Init();
    CompressingInfo.Init();
    ProcessedSize = 0;
    
    DecompressingInfoTemp.Init();
    DecompressingInfo.Init();

    NumPasses = 0;
    // NumErrors = 0;
  }
  void Stop()
  {
    NWindows::NSynchronization::CCriticalSectionLock lock(CS);
    Stopped = true;
  }
  bool WasStopped()
  {
    NWindows::NSynchronization::CCriticalSectionLock lock(CS);
    return Stopped;
  }
  void Pause()
  {
    NWindows::NSynchronization::CCriticalSectionLock lock(CS);
    Paused = true;
  }
  void Start()
  {
    NWindows::NSynchronization::CCriticalSectionLock lock(CS);
    Paused = false;
  }
  bool WasPaused()
  {
    NWindows::NSynchronization::CCriticalSectionLock lock(CS);
    return Paused;
  }
  void WaitCreating() { _startEvent.Lock(); }
};

class CBenchmarkDialog:
  public NWindows::NControl::CModalDialog
{
  NWindows::NControl::CComboBox m_Dictionary;
  NWindows::NControl::CComboBox m_NumThreads;
  UINT_PTR _timer;
  UINT32 _startTime;

  bool OnTimer(WPARAM timerID, LPARAM callback);
  virtual bool OnInit();
  void OnRestartButton();
  void OnStopButton();
  void OnHelp();
  virtual void OnCancel();
  bool OnButtonClicked(int buttonID, HWND buttonHWND);
  bool OnCommand(int code, int itemID, LPARAM lParam);

  void PrintTime();
  void PrintRating(UInt64 rating, UINT controlID);
  void PrintUsage(UInt64 usage, UINT controlID);
  void PrintResults(
      UINT32 dictionarySize,
      const CBenchInfo2 &info, UINT usageID, UINT speedID, UINT rpuID, UINT ratingID,
      bool decompressMode = false);

  UInt32 GetNumberOfThreads();
  UInt32 OnChangeDictionary();
  void OnChangeSettings();
public:
  CProgressSyncInfo Sync;

  CBenchmarkDialog(): _timer(0) {}
  INT_PTR Create(HWND wndParent = 0)
  {
    BIG_DIALOG_SIZE(332, 228);
    return CModalDialog::Create(SIZED_DIALOG(IDD_DIALOG_BENCHMARK), wndParent);
  }
  void MessageBoxError(LPCWSTR message)
  {
    MessageBoxW(*this, message, L"7-Zip", MB_ICONERROR);
  }
};

HRESULT Benchmark(
    DECL_EXTERNAL_CODECS_LOC_VARS
    UInt32 numThreads, UInt32 dictionarySize, HWND hwndParent = NULL);

#endif

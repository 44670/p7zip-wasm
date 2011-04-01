// ListViewDialog.cpp

#include "StdAfx.h"

#include "ListViewDialog.h"
#include "RegistryUtils.h"

#ifdef LANG
#include "LangUtils.h"
static CIDLangPair kIDLangPairs[] =
{
  { IDOK, 0x02000702 },
  { IDCANCEL, 0x02000710 }
};
#endif

bool CListViewDialog::OnInit()
{
  #ifdef LANG
  LangSetDlgItemsText(HWND(*this), kIDLangPairs, sizeof(kIDLangPairs) / sizeof(kIDLangPairs[0]));
  #endif
  _listView.Attach(GetItem(IDC_LISTVIEW_LIST));

//FIXME  if (ReadSingleClick())
//FIXME    _listView.SetExtendedListViewStyle(LVS_EX_ONECLICKACTIVATE | LVS_EX_TRACKSELECT);

  SetText(Title);

  LVCOLUMN columnInfo;
  columnInfo.mask = LVCF_FMT | LVCF_WIDTH | LVCF_SUBITEM;
  columnInfo.fmt = LVCFMT_LEFT;
  columnInfo.iSubItem = 0;
  columnInfo.cx = 200;

  _listView.InsertColumn(0, &columnInfo);

  for (int i = 0; i < Strings.Size(); i++)
    _listView.InsertItem(i, Strings[i]);

  if (Strings.Size() > 0)
    _listView.SetItemState_FocusedSelected(0);

  _listView.SetColumnWidthAuto(0);
  StringsWereChanged = false;

  NormalizeSize();
  return CModalDialog::OnInit();
}

#ifdef _WIN32 // FIXME
bool CListViewDialog::OnNotify(UINT /* controlID */, LPNMHDR header)
{
  if (header->hwndFrom != _listView)
    return false;
  switch(header->code)
  {
    case LVN_ITEMACTIVATE:
      if (g_LVN_ITEMACTIVATE_Support)
      {
        OnOK();
        return true;
      }
      break;
    case NM_DBLCLK:
    case NM_RETURN: // probabably it's unused
      if (!g_LVN_ITEMACTIVATE_Support)
      {
        OnOK();
        return true;
      }
      break;

    case LVN_KEYDOWN:
    {
      LPNMLVKEYDOWN keyDownInfo = LPNMLVKEYDOWN(header);
      switch(keyDownInfo->wVKey)
      {
        case VK_DELETE:
        {
          if (!DeleteIsAllowed)
            return false;
          for (;;)
          {
            int index = _listView.GetNextSelectedItem(-1);
            if (index < 0)
              break;
            StringsWereChanged = true;
            _listView.DeleteItem(index);
            Strings.Delete(index);
          }
          int focusedIndex = _listView.GetFocusedItem();
          if (focusedIndex >= 0)
            _listView.SetItemState_FocusedSelected(focusedIndex);
          _listView.SetColumnWidthAuto(0);
          return true;
        }
        case 'A':
        {
          bool ctrl = (::GetKeyState(VK_CONTROL) & 0x8000) != 0;
          if (ctrl)
          {
            int numItems = _listView.GetItemCount();
            for (int i = 0; i < numItems; i++)
              _listView.SetItemState(i, LVIS_SELECTED, LVIS_SELECTED);
            return true;
          }
        }
      }
    }
  }
  return false;
}
#endif

void CListViewDialog::OnOK()
{
  FocusedItemIndex = _listView.GetFocusedItem();
  CModalDialog::OnOK();
}

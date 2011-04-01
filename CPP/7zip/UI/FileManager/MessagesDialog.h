// MessagesDialog.h

#ifndef __MESSAGES_DIALOG_H
#define __MESSAGES_DIALOG_H

#include "Windows/Control/Dialog.h"
#include "Windows/Control/ListView.h"

#include "MessagesDialogRes.h"

class CMessagesDialog: public NWindows::NControl::CModalDialog
{
  NWindows::NControl::CListView _messageList;
  void AddMessageDirect(LPCWSTR message);
  void AddMessage(LPCWSTR message);
  virtual bool OnInit();
public:
  const UStringVector *Messages;
  INT_PTR Create(HWND parent = 0) { return CModalDialog::Create(IDD_DIALOG_MESSAGES, parent); }
};

#endif

/*
* Copyright 2025 Ingemar Hedvall
* SPDX-License-Identifier: MIT
 */

#pragma once
#include <wx/wx.h>
#include <wx/choice.h>


namespace bus {

class AddLogMessageDialog : public wxDialog {
 public:
  AddLogMessageDialog(wxWindow *parent);

 protected:

 private:
  wxChoice* severity_ctrl_ = nullptr;
  wxTextCtrl* message_ctrl_ = nullptr;

  void OnSend(wxCommandEvent& event);
  wxDECLARE_EVENT_TABLE();
};


}  // namespace bus


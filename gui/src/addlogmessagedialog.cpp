/*
* Copyright 2025 Ingemar Hedvall
* SPDX-License-Identifier: MIT
 */

#include "addlogmessagedialog.h"

#include <util/logmessage.h>
#include <util/logconfig.h>

#include <wx/valgen.h>
#include <wx/choice.h>
#include "windowid.h"

using namespace util::log;

namespace {
const wxArrayString kSeverityList = {
    wxString("Trace"),
    wxString("Debug"),
    wxString("Information"),
    wxString("Notice"),
    wxString("Warning"),
    wxString("Error"),
    wxString("Critical"),
    wxString("Alert"),
    wxString("Emergency"),
};

}

namespace bus {

wxBEGIN_EVENT_TABLE(AddLogMessageDialog, wxDialog) //NOLINT
  EVT_BUTTON(kIdSendLogMessage, AddLogMessageDialog::OnSend)
wxEND_EVENT_TABLE()

AddLogMessageDialog::AddLogMessageDialog(wxWindow *parent)
    : wxDialog(parent,wxID_ANY,"Select the Database Type",wxDefaultPosition,
               wxDefaultSize) {
  auto* close_button = new wxButton(this, wxID_OK, wxGetStockLabel(wxID_OK));
  auto* send_button = new wxButton(this, kIdSendLogMessage, "Send Log Message");


  severity_ctrl_ = new wxChoice(this, wxID_ANY, wxDefaultPosition,wxDefaultSize,
                            kSeverityList, 0);
  severity_ctrl_->SetSelection(0);
    severity_ctrl_->SetMinSize({20*8,-1});

  message_ctrl_ = new wxTextCtrl(this, wxID_ANY, wxString("Hello Log Message"));
  message_ctrl_->SetMinSize({80*8,-1});

  auto* severity_label = new wxStaticText(this, wxID_ANY, L"Severity:");
  auto* message_label = new wxStaticText(this, wxID_ANY, L"Message:");

  int label_width = 100;
  label_width = std::max(label_width, severity_label->GetBestSize().GetX());
  label_width = std::max(label_width, message_label->GetBestSize().GetX());

  auto* severity_sizer = new wxBoxSizer(wxHORIZONTAL);
  severity_label->SetMinSize({label_width, -1});
  severity_sizer->Add(severity_label, 0, wxALIGN_CENTER_VERTICAL | wxLEFT, 5);
  severity_sizer->Add(severity_ctrl_, 0, wxALIGN_CENTER_VERTICAL |
                                             wxLEFT | wxRIGHT, 5);
  severity_sizer->Add(send_button, 0, wxALIGN_CENTER_VERTICAL |
                                             wxLEFT | wxRIGHT, 5);
  auto* message_sizer = new wxBoxSizer(wxHORIZONTAL);
  message_label->SetMinSize({label_width, -1});
  message_sizer->Add(message_label, 0, wxALIGN_CENTER_VERTICAL | wxLEFT, 5);
  message_sizer->Add(message_ctrl_, 0, wxALIGN_CENTER_VERTICAL |
                                             wxLEFT | wxRIGHT, 5);


  auto* system_sizer = new wxStdDialogButtonSizer();
  system_sizer->AddButton(close_button);
  system_sizer->Realize();

  auto* main_sizer = new wxBoxSizer(wxVERTICAL);
  main_sizer->Add(severity_sizer, 0, wxALIGN_LEFT | wxTOP | wxBOTTOM | wxEXPAND, 4);
  main_sizer->Add(message_sizer, 0, wxALIGN_LEFT | wxTOP | wxBOTTOM | wxEXPAND, 4);
  main_sizer->Add(system_sizer, 0, wxALIGN_CENTER_HORIZONTAL | wxBOTTOM | wxLEFT | wxRIGHT, 10);

  SetSizerAndFit(main_sizer);
  send_button->SetDefault();

}

void AddLogMessageDialog::OnSend(wxCommandEvent &event) {

  const wxString severity_str = severity_ctrl_->GetStringSelection();
  LogSeverity severity = LogSeverity::kTrace;
  for (size_t index = 0; index < kSeverityList.size(); ++index) {
    if (severity_str == kSeverityList[index]) {
      severity = static_cast<LogSeverity>(index);
      break;
    }
  }
  const wxString message_str = message_ctrl_->GetValue();
  LogMessage msg;
  msg.severity = severity;
  msg.message = message_str.ToStdString();
  LogConfig::Instance().AddLogMessage(msg);

}

}  // namespace bus

/*
* Copyright 2025 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */

#include "unknownsourcedialog.h"

#include <wx/config.h>
#include <wx/valnum.h>

#include <algorithm>
#include <filesystem>

#include "busmaster.h"
#include "util/stringutil.h"
#include "windowid.h"

using namespace std::filesystem;
using namespace util::string;

namespace bus {
wxBEGIN_EVENT_TABLE(UnknownSourceDialog, wxDialog)
  EVT_UPDATE_UI(wxID_SAVE, UnknownSourceDialog::OnUpdateSave)
  EVT_BUTTON(wxID_SAVE, UnknownSourceDialog::OnSave)
  EVT_TEXT(kIdSave, UnknownSourceDialog::OnNameChange)
wxEND_EVENT_TABLE()

UnknownSourceDialog::UnknownSourceDialog(wxWindow *parent)
    : wxDialog(parent,wxID_ANY,"Environment Dialog") {
  auto* save_button = new wxButton(this, wxID_SAVE, wxGetStockLabel(wxID_SAVE));
  auto* cancel_button = new wxButton(this, wxID_CANCEL, wxGetStockLabel(wxID_CANCEL));

  name_ctrl_ = new wxTextCtrl(this, kIdSave, wxEmptyString,
                              wxDefaultPosition, wxDefaultSize,
                              wxTE_LEFT | wxTE_PROCESS_ENTER,
                              wxTextValidator(wxFILTER_EMPTY | wxFILTER_ASCII, &name_));
  name_ctrl_->SetMaxLength(40);
  name_ctrl_->SetMinSize({40*8,-1});

  auto* description = new wxTextCtrl(this, wxID_ANY, wxEmptyString,
                                     wxDefaultPosition, wxDefaultSize,
                                     wxTE_LEFT,
                                     wxTextValidator(wxFILTER_NONE, &description_));
  description->SetMinSize({80*8,-1});

  auto* name_label = new wxStaticText(this, wxID_ANY, L"Name:");
  auto* description_label = new wxStaticText(this, wxID_ANY, L"Description:");

  int label_width = 100;
  label_width = std::max(label_width,name_label->GetBestSize().GetX());
  label_width = std::max(label_width, description_label->GetBestSize().GetX());

  auto* name_sizer = new wxBoxSizer(wxHORIZONTAL);
  name_label->SetMinSize({label_width, -1});
  name_sizer->Add(name_label, 0, wxALIGN_CENTER_VERTICAL | wxLEFT, 5);
  name_sizer->Add(name_ctrl_, 0, wxALIGN_CENTER_VERTICAL | wxLEFT | wxRIGHT, 5);

  auto* description_sizer = new wxBoxSizer(wxHORIZONTAL);
  description_label->SetMinSize({label_width, -1});
  description_sizer->Add(description_label, 0, wxALIGN_CENTER_VERTICAL | wxLEFT , 5);
  description_sizer->Add(description, 1,  wxLEFT | wxRIGHT | wxEXPAND, 5);

  auto* system_sizer = new wxStdDialogButtonSizer();
  system_sizer->AddButton(save_button);
  system_sizer->AddButton(cancel_button);
  system_sizer->Realize();

  auto* main_sizer = new wxBoxSizer(wxVERTICAL);
  main_sizer->Add(name_sizer, 0, wxALIGN_LEFT | wxTOP | wxBOTTOM | wxEXPAND, 4);
  main_sizer->Add(description_sizer, 0, wxALIGN_LEFT | wxBOTTOM | wxEXPAND, 4);
  main_sizer->Add(system_sizer, 0, wxALIGN_CENTER_HORIZONTAL | wxBOTTOM | wxLEFT | wxRIGHT, 10);

  SetSizerAndFit(main_sizer);
  cancel_button->SetDefault();

}
void UnknownSourceDialog::SetInvalidNames(
    std::vector<std::string>& invalid_names) {
  invalid_names = std::move(invalid_names);
}

void UnknownSourceDialog::SetSource( const ISource& source) {
  name_ = source.Name();
  description_ = source.Description();

  TransferDataToWindow();
}

bool UnknownSourceDialog::GetSource(ISource& source) {
  TransferDataFromWindow();
  bool modified = false;
  if (source.Name() != name_.ToStdString()) {
    source.Name(name_.ToStdString());
    modified = true;
  }
  if (source.Description() != description_.ToStdString()) {
    source.Description(description_.ToStdString());
    modified = true;
  }
  return modified;
}

void UnknownSourceDialog::OnSave(wxCommandEvent &event) {
  if (!Validate() || !TransferDataFromWindow()) {
    return;
  }
  if (!IsValidName()) {
    std::wostringstream msg;
    msg << "A similar name already exists." << std::endl;
    msg << "Select a different name." << std::endl;
    wxMessageBox(msg.str(), "Invalid Name",
                 wxOK | wxCENTRE | wxICON_ERROR,
                 this);
    return;
  }
  if (IsModal()) {
    EndModal(wxID_SAVE);
  } else {
    SetReturnCode(wxID_SAVE);
    Show(false);
  }
}

void UnknownSourceDialog::OnUpdateSave(wxUpdateUIEvent &event) {
  event.Enable(!name_.IsEmpty());
}

void UnknownSourceDialog::OnNameChange(wxCommandEvent&) {
  name_ctrl_->TransferDataFromWindow();
}

bool UnknownSourceDialog::TransferDataToWindow() {
  return wxWindowBase::TransferDataToWindow();
}

bool UnknownSourceDialog::TransferDataFromWindow() {  // Fix the exe_picker and combo boxes
  auto ret = wxWindowBase::TransferDataFromWindow();
  name_.Trim(true).Trim(false);
  description_.Trim(true).Trim(false);
  return ret;
}

bool UnknownSourceDialog::IsValidName() const {
  const std::string ref_name = name_.ToStdString();
  const bool valid = std::ranges::none_of( invalid_names_,
                                          [&] (const std::string& name) -> bool {
                                            return IEquals(name, ref_name);
                                          });
  return valid;
}

} // bus
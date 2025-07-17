/*
* Copyright 2025 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */

#include "databasedialog.h"

#include <wx/config.h>
#include <wx/valnum.h>

#include <algorithm>
#include <filesystem>

#include "util/stringutil.h"

#include "busmaster.h"
#include "windowid.h"

using namespace std::filesystem;
using namespace util::string;

namespace bus {
wxBEGIN_EVENT_TABLE(DatabaseDialog, wxDialog)
    EVT_UPDATE_UI(wxID_SAVE, DatabaseDialog::OnUpdateSave)
    EVT_BUTTON(wxID_SAVE, DatabaseDialog::OnSave)
    EVT_FILEPICKER_CHANGED(kIdConfigPicker, DatabaseDialog::OnConfigPicker)
    EVT_TEXT(kIdSave, DatabaseDialog::OnNameChange)
wxEND_EVENT_TABLE()

DatabaseDialog::DatabaseDialog(wxWindow *parent)
    : wxDialog(parent,wxID_ANY,"Database Dialog") {
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

  file_picker_ = new wxFilePickerCtrl(this, kIdConfigPicker, wxEmptyString,
                                      file_filter_, default_extension_,
                                      wxDefaultPosition, wxDefaultSize,
                                      wxFLP_OPEN | wxFLP_FILE_MUST_EXIST | wxFLP_USE_TEXTCTRL | wxFLP_SMALL);
  file_picker_->SetMinSize({80*8,-1});

  // Fetch initial directory
  const auto& app = wxGetApp();
  const wxString app_name = app.GetAppName();

  const auto* config = wxConfig::Get();
  if (config != nullptr) {
    const wxString app_dir = config->ReadObject(
        wxString("/DatabaseDialog/Path"), wxString("") );
    if (!app_dir.empty()) {
      file_picker_->SetInitialDirectory(app_dir);
    }
  }

  auto* name_label = new wxStaticText(this, wxID_ANY, L"Name:");
  auto* description_label = new wxStaticText(this, wxID_ANY, L"Description:");
  auto* file_label = new wxStaticText(this, wxID_ANY, L"Database File:");

  int label_width = 100;
  label_width = std::max(label_width,name_label->GetBestSize().GetX());
  label_width = std::max(label_width, description_label->GetBestSize().GetX());
  label_width = std::max(label_width, file_label->GetBestSize().GetX());

  auto* name_sizer = new wxBoxSizer(wxHORIZONTAL);
  name_label->SetMinSize({label_width, -1});
  name_sizer->Add(name_label, 0, wxALIGN_CENTER_VERTICAL | wxLEFT, 5);
  name_sizer->Add(name_ctrl_, 0, wxALIGN_CENTER_VERTICAL | wxLEFT | wxRIGHT, 5);

  auto* description_sizer = new wxBoxSizer(wxHORIZONTAL);
  description_label->SetMinSize({label_width, -1});
  description_sizer->Add(description_label, 0, wxALIGN_CENTER_VERTICAL | wxLEFT , 5);
  description_sizer->Add(description, 1,  wxLEFT | wxRIGHT | wxEXPAND, 5);

  auto* file_sizer = new wxBoxSizer(wxHORIZONTAL);
  file_label->SetMinSize({label_width, -1});
  file_sizer->Add(file_label, 0, wxALIGN_CENTER_VERTICAL | wxLEFT , 5);
  file_sizer->Add(file_picker_, 0, wxALIGN_CENTER_VERTICAL | wxLEFT | wxRIGHT, 5);

  auto* system_sizer = new wxStdDialogButtonSizer();
  system_sizer->AddButton(save_button);
  system_sizer->AddButton(cancel_button);
  system_sizer->Realize();

  auto* main_sizer = new wxBoxSizer(wxVERTICAL);
  main_sizer->Add(name_sizer, 0, wxALIGN_LEFT | wxTOP | wxBOTTOM | wxEXPAND, 4);
  main_sizer->Add(description_sizer, 0, wxALIGN_LEFT | wxBOTTOM | wxEXPAND, 4);
  main_sizer->Add(file_sizer, 0, wxALIGN_LEFT | wxBOTTOM | wxEXPAND, 4);

  main_sizer->Add(system_sizer, 0,
                  wxALIGN_CENTER_HORIZONTAL | wxBOTTOM | wxLEFT | wxRIGHT, 10);

  SetSizerAndFit(main_sizer);
  cancel_button->SetDefault();

}
void DatabaseDialog::SetInvalidNames(
    std::vector<std::string>& invalid_names) {
  invalid_names = std::move(invalid_names);
}

void DatabaseDialog::SetDatabase( const IDatabase& database) {
  name_ = database.Name();
  description_ = database.Description();
  filename_ = database.Filename();
  file_filter_ = database.FileFilter();
  default_extension_ = database.DefaultExtension();
  TransferDataToWindow();
}

bool DatabaseDialog::GetDatabase(IDatabase& database) {
  TransferDataFromWindow();
  bool modified = false;
  if (database.Name() != name_.ToStdString()) {
    database.Name(name_.ToStdString());
    modified = true;
  }
  if (database.Description() != description_.ToStdString()) {
    database.Description(description_.ToStdString());
    modified = true;
  }
  if (database.Filename() != filename_.ToStdString()) {
    database.Filename(filename_.ToStdString());
    modified = true;
  }
  return modified;
}

void DatabaseDialog::OnSave(wxCommandEvent &event) {
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

void DatabaseDialog::OnUpdateSave(wxUpdateUIEvent &event) {
  event.Enable(!name_.IsEmpty());
}

void DatabaseDialog::OnConfigPicker(wxFileDirPickerEvent& event) {
  const wxString file = event.GetPath();
  try {
    std::filesystem::path full_name(file.ToStdWstring());
    if (auto* config = wxConfig::Get();
        config != nullptr) {
      config->Write("/DatabaseDialog/Path",
                    wxString(full_name.parent_path().wstring()));
    }
  } catch (const std::exception&) {
  }
}

void DatabaseDialog::OnNameChange(wxCommandEvent&) {
  name_ctrl_->TransferDataFromWindow();
}

bool DatabaseDialog::TransferDataToWindow() {
  std::wostringstream app_arg;
  file_picker_->SetPath(filename_);
  return wxWindowBase::TransferDataToWindow();
}

bool DatabaseDialog::TransferDataFromWindow() {  // Fix the exe_picker and combo boxes
  filename_ = file_picker_->GetPath();
  auto ret = wxWindowBase::TransferDataFromWindow();
  name_.Trim(true).Trim(false);
  description_.Trim(true).Trim(false);
  filename_.Trim(true).Trim(false);
  return ret;
}

bool DatabaseDialog::IsValidName() const {
  const std::string ref_name = name_.ToStdString();
  const bool valid = std::ranges::none_of( invalid_names_,
                                          [&] (const std::string& name) -> bool {
                                            return IEquals(name, ref_name);
                                          });
  return valid;
}

} // bus
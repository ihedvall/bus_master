/*
* Copyright 2025 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */

#include "projectdialog.h"

#include <filesystem>

#include <wx/config.h>
#include <wx/valgen.h>

#include "busmaster.h"
#include "util/systeminfo.h"
#include "windowid.h"

using namespace util::supervise;
using namespace std::filesystem;

namespace bus {
wxBEGIN_EVENT_TABLE(ProjectDialog, wxDialog) //NOLINT
  EVT_UPDATE_UI(wxID_SAVE, ProjectDialog::OnUpdateSave)
  EVT_BUTTON(wxID_SAVE, ProjectDialog::OnSave)
  EVT_FILEPICKER_CHANGED(kIdConfigPicker, ProjectDialog::OnConfigPicker)
  EVT_TEXT(kIdSave, ProjectDialog::OnNameChange)
wxEND_EVENT_TABLE()

ProjectDialog::ProjectDialog(wxWindow *parent)
: wxDialog(parent,wxID_ANY,"Project Dialog") {
  auto* save_button = new wxButton(this, wxID_SAVE, wxGetStockLabel(wxID_SAVE));
  auto* cancel_button = new wxButton(this, wxID_CANCEL, wxGetStockLabel(wxID_CANCEL));

  name_ctrl_ = new wxTextCtrl(this, kIdSave, wxEmptyString,wxDefaultPosition,wxDefaultSize,
                         wxTE_LEFT | wxTE_PROCESS_ENTER,
                         wxTextValidator(wxFILTER_EMPTY | wxFILTER_ASCII, &name_));
  name_ctrl_->SetMaxLength(40);
  name_ctrl_->SetMinSize({40*8,-1});

  desc_ctrl_ = new wxTextCtrl(this, wxID_ANY, wxEmptyString,wxDefaultPosition,wxDefaultSize,
      wxTE_LEFT,wxTextValidator(wxFILTER_NONE, &description_));
  desc_ctrl_->SetMinSize({80*8,-1});

  config_picker_ = new wxFilePickerCtrl(this, kIdConfigPicker, wxEmptyString,
                                     L"Project File (*.xml)|*.xml|All Files (*.*)|*.*", "*.xml",
                                     wxDefaultPosition, wxDefaultSize,
                                     wxFLP_OPEN | wxFLP_USE_TEXTCTRL | wxFLP_SMALL);
  config_picker_->SetMinSize({80*8,-1});

  // Fetch initial directory
  auto& app = wxGetApp();
  wxString app_name = app.GetAppName();
  SystemInfo system_info(app_name.ToStdString());
  const auto* config = wxConfig::Get();
  if (config != nullptr) {
    const wxString app_dir = config->ReadObject(
      wxString("/ProjectDialog/Path"),
      wxString(system_info.ConfigPath()) );
    if (!app_dir.empty()) {
      config_picker_->SetInitialDirectory(app_dir);
    }
  }

  auto* name_label = new wxStaticText(this, wxID_ANY, L"Name:");
  auto* description_label = new wxStaticText(this, wxID_ANY, L"Description:");
  auto* config_label = new wxStaticText(this, wxID_ANY, L"Configuration File:");

  int label_width = 100;
  label_width = std::max(label_width,name_label->GetBestSize().GetX());
  label_width = std::max(label_width, description_label->GetBestSize().GetX());
  label_width = std::max(label_width, config_label->GetBestSize().GetX());

  auto* name_sizer = new wxBoxSizer(wxHORIZONTAL);
  name_label->SetMinSize({label_width, -1});
  name_sizer->Add(name_label, 0, wxALIGN_CENTER_VERTICAL | wxLEFT, 5);
  name_sizer->Add(name_ctrl_, 0, wxALIGN_CENTER_VERTICAL | wxLEFT | wxRIGHT, 5);

  auto* description_sizer = new wxBoxSizer(wxHORIZONTAL);
  description_label->SetMinSize({label_width, -1});
  description_sizer->Add(description_label, 0, wxALIGN_CENTER_VERTICAL | wxLEFT , 5);
  description_sizer->Add(desc_ctrl_, 1,  wxLEFT | wxRIGHT | wxEXPAND, 5);

  auto* config_sizer = new wxBoxSizer(wxHORIZONTAL);
  config_label->SetMinSize({label_width, -1});
  config_sizer->Add(config_label, 0, wxALIGN_CENTER_VERTICAL | wxLEFT , 5);
  config_sizer->Add(config_picker_, 0, wxALIGN_CENTER_VERTICAL | wxLEFT | wxRIGHT, 5);

  auto* system_sizer = new wxStdDialogButtonSizer();
  system_sizer->AddButton(save_button);
  system_sizer->AddButton(cancel_button);
  system_sizer->Realize();


  auto* main_sizer = new wxBoxSizer(wxVERTICAL);
  main_sizer->Add(name_sizer, 0, wxALIGN_LEFT | wxTOP | wxBOTTOM | wxEXPAND, 4);
  main_sizer->Add(description_sizer, 0, wxALIGN_LEFT | wxBOTTOM | wxEXPAND, 4);
  main_sizer->Add(config_sizer, 0, wxALIGN_LEFT | wxBOTTOM | wxEXPAND, 4);
  main_sizer->Add(system_sizer, 0, wxALIGN_CENTER_HORIZONTAL | wxBOTTOM | wxLEFT | wxRIGHT, 10);

  SetSizerAndFit(main_sizer);
  cancel_button->SetDefault();

}

void ProjectDialog::OnSave(wxCommandEvent &event) {
  if (!Validate() || !TransferDataFromWindow()) {
    return;
  }
  if (IsModal()) {
    EndModal(wxID_SAVE);
  } else {
    SetReturnCode(wxID_SAVE);
    Show(false);
  }
}

void ProjectDialog::OnUpdateSave(wxUpdateUIEvent &event) {
  event.Enable(!name_.IsEmpty());
}

void ProjectDialog::OnConfigPicker(wxFileDirPickerEvent& event) {
  const wxString file = event.GetPath();
  try {
    path full_name(file.ToStdWstring());
    if (auto* config = wxConfig::Get();
        config != nullptr) {
      config->Write("/ProjectDialog/Path",
        wxString(full_name.parent_path().wstring()));
    }
    // Check if the selected file exist and if so read in any name
    // and description
    if (exists(full_name)) {
      Project temp;
      temp.ConfigFile(full_name.string());

      const bool read = temp.ReadConfig();
      if (read && name_ctrl_ != nullptr && name_.IsEmpty()) {
        name_ctrl_->SetValue( temp.Name() );
      }
      if (read && desc_ctrl_ != nullptr && description_.IsEmpty()) {
        desc_ctrl_->SetValue(temp.Description());
      }
    }

  } catch (const std::exception&) {
  }
}

void ProjectDialog::OnNameChange(wxCommandEvent&) {
  name_ctrl_->TransferDataFromWindow();
}

void ProjectDialog::SetProject(const Project& project) {
  name_ = project.Name();
  description_ = project.Description();
  config_file_ = project.ConfigFile();
  TransferDataToWindow();
}

bool ProjectDialog::GetProject(Project& project) {
  TransferDataFromWindow();
  bool modified = false;
  if (project.Name() != name_.ToStdString()) {
    modified = true;
    project.Name(name_.ToStdString());
  }

  if (project.Description() != description_.ToStdString()) {
    modified = true;
    project.Description(description_.ToStdString());
  }

  if (project.ConfigFile() != config_file_.ToStdString()) {
    modified = true;
    project.ConfigFile(config_file_.ToStdString());
  }

  return modified;
}

bool ProjectDialog::TransferDataToWindow() {
  config_picker_->SetPath(config_file_);
  return wxWindowBase::TransferDataToWindow();
}

bool ProjectDialog::TransferDataFromWindow() {  // Fix the exe_picker and combo boxes
  config_file_ = config_picker_->GetPath();
  const auto ret = wxWindowBase::TransferDataFromWindow();
  name_.Trim(true).Trim(false);
  description_.Trim(true).Trim(false);
  config_file_.Trim(true).Trim(false);
  return ret;
}



} // bus
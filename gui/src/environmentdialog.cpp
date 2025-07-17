/*
* Copyright 2025 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */

#include "environmentdialog.h"

#include <wx/config.h>
#include <wx/valnum.h>

#include <algorithm>
#include <filesystem>

#include "busmaster.h"
#include "util/isuperviseapplication.h"
#include "util/stringutil.h"
#include "windowid.h"

using namespace std::filesystem;
using namespace util::string;

namespace bus {
wxBEGIN_EVENT_TABLE(EnvironmentDialog, wxDialog)
    EVT_UPDATE_UI(wxID_SAVE, EnvironmentDialog::OnUpdateSave)
    EVT_BUTTON(wxID_SAVE, EnvironmentDialog::OnSave)
    EVT_FILEPICKER_CHANGED(kIdConfigPicker, EnvironmentDialog::OnConfigPicker)
    EVT_TEXT(kIdSave, EnvironmentDialog::OnNameChange)
wxEND_EVENT_TABLE()

EnvironmentDialog::EnvironmentDialog(wxWindow *parent)
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

  config_picker_ = new wxFilePickerCtrl(this, kIdConfigPicker, wxEmptyString,
           L"Configuration Files (*.xml)|*.xml|All Files (*.*)|*.*", "*.xml",
                                        wxDefaultPosition, wxDefaultSize,
        wxFLP_OPEN | wxFLP_FILE_MUST_EXIST | wxFLP_USE_TEXTCTRL | wxFLP_SMALL);
  config_picker_->SetMinSize({80*8,-1});

  wxTextValidator sh_validator(
      wxFILTER_ALPHANUMERIC | wxFILTER_EXCLUDE_CHAR_LIST, &shared_memory_);
  sh_validator.SetCharExcludes(" ");
  auto* shared_memory = new wxTextCtrl(this, wxID_ANY, wxEmptyString,
                                       wxDefaultPosition,wxDefaultSize,
                                       wxTE_LEFT,sh_validator);
  shared_memory->SetMinSize({20*8,-1});

  auto* host_name = new wxTextCtrl(this, wxID_ANY, wxEmptyString,
                                   wxDefaultPosition,wxDefaultSize, wxTE_LEFT,
                                   wxTextValidator(wxFILTER_NONE, &host_name_));
  host_name->SetMinSize({20*8,-1});

  auto* port = new wxTextCtrl(this, wxID_ANY, wxEmptyString,
                              wxDefaultPosition, wxDefaultSize, wxTE_LEFT,
                              wxIntegerValidator<uint16_t>(&port_));
  port->SetMinSize({20*8,-1});
  // Fetch initial directory
  const auto& app = wxGetApp();
  const wxString app_name = app.GetAppName();

  const auto* config = wxConfig::Get();
  if (config != nullptr) {
    const wxString app_dir = config->ReadObject(
                wxString("/EnvironmentDialog/Path"), wxString("") );
    if (!app_dir.empty()) {
      config_picker_->SetInitialDirectory(app_dir);
    }
  }

  auto* name_label = new wxStaticText(this, wxID_ANY, L"Name:");
  auto* description_label = new wxStaticText(this, wxID_ANY, L"Description:");
  auto* config_label = new wxStaticText(this, wxID_ANY, L"Configuration File:");
  auto* shared_label = new wxStaticText(this, wxID_ANY, L"Shared Memory Name:");
  auto* host_label = new wxStaticText(this, wxID_ANY, L"Host Name:");
  auto* port_label = new wxStaticText(this, wxID_ANY, L"TCP/IP Port:");

  int label_width = 100;
  label_width = std::max(label_width,name_label->GetBestSize().GetX());
  label_width = std::max(label_width, description_label->GetBestSize().GetX());
  label_width = std::max(label_width, config_label->GetBestSize().GetX());
  label_width = std::max(label_width, shared_label->GetBestSize().GetX());
  label_width = std::max(label_width, host_label->GetBestSize().GetX());
  label_width = std::max(label_width, port_label->GetBestSize().GetX());

  auto* name_sizer = new wxBoxSizer(wxHORIZONTAL);
  name_label->SetMinSize({label_width, -1});
  name_sizer->Add(name_label, 0, wxALIGN_CENTER_VERTICAL | wxLEFT, 5);
  name_sizer->Add(name_ctrl_, 0, wxALIGN_CENTER_VERTICAL | wxLEFT | wxRIGHT, 5);

  auto* description_sizer = new wxBoxSizer(wxHORIZONTAL);
  description_label->SetMinSize({label_width, -1});
  description_sizer->Add(description_label, 0, wxALIGN_CENTER_VERTICAL | wxLEFT , 5);
  description_sizer->Add(description, 1,  wxLEFT | wxRIGHT | wxEXPAND, 5);

  auto* config_sizer = new wxBoxSizer(wxHORIZONTAL);
  config_label->SetMinSize({label_width, -1});
  config_sizer->Add(config_label, 0, wxALIGN_CENTER_VERTICAL | wxLEFT , 5);
  config_sizer->Add(config_picker_, 0, wxALIGN_CENTER_VERTICAL | wxLEFT | wxRIGHT, 5);

  auto* shared_sizer = new wxBoxSizer(wxHORIZONTAL);
  shared_label->SetMinSize({label_width, -1});
  shared_sizer->Add(shared_label, 0, wxALIGN_CENTER_VERTICAL | wxLEFT , 5);
  shared_sizer->Add(shared_memory, 0, wxALIGN_CENTER_VERTICAL | wxLEFT | wxRIGHT, 5);

  auto* host_sizer = new wxBoxSizer(wxHORIZONTAL);
  host_label->SetMinSize({label_width, -1});
  host_sizer->Add(host_label, 0, wxALIGN_CENTER_VERTICAL | wxLEFT , 5);
  host_sizer->Add(host_name, 0, wxALIGN_CENTER_VERTICAL | wxLEFT | wxRIGHT, 5);

  auto* port_sizer = new wxBoxSizer(wxHORIZONTAL);
  port_label->SetMinSize({label_width, -1});
  port_sizer->Add(port_label, 0, wxALIGN_CENTER_VERTICAL | wxLEFT , 5);
  port_sizer->Add(port, 0, wxALIGN_CENTER_VERTICAL | wxLEFT | wxRIGHT, 5);

  auto* system_sizer = new wxStdDialogButtonSizer();
  system_sizer->AddButton(save_button);
  system_sizer->AddButton(cancel_button);
  system_sizer->Realize();

  auto* main_sizer = new wxBoxSizer(wxVERTICAL);
  main_sizer->Add(name_sizer, 0, wxALIGN_LEFT | wxTOP | wxBOTTOM | wxEXPAND, 4);
  main_sizer->Add(description_sizer, 0, wxALIGN_LEFT | wxBOTTOM | wxEXPAND, 4);
  main_sizer->Add(config_sizer, 0, wxALIGN_LEFT | wxBOTTOM | wxEXPAND, 4);
  main_sizer->Add(shared_sizer, 0, wxALIGN_LEFT | wxBOTTOM | wxEXPAND, 4);
  main_sizer->Add(host_sizer, 0, wxALIGN_LEFT | wxBOTTOM | wxEXPAND, 4);
  main_sizer->Add(port_sizer, 0, wxALIGN_LEFT | wxBOTTOM | wxEXPAND, 4);
  main_sizer->Add(system_sizer, 0, wxALIGN_CENTER_HORIZONTAL | wxBOTTOM | wxLEFT | wxRIGHT, 10);

  SetSizerAndFit(main_sizer);
  cancel_button->SetDefault();

}
void EnvironmentDialog::SetInvalidNames(
    std::vector<std::string>& invalid_names) {
  invalid_names = std::move(invalid_names);
}

void EnvironmentDialog::SetEnvironment( const IEnvironment& environment) {
  name_ = environment.Name();
  description_ = environment.Description();
  config_file_ = environment.ConfigFile();

  shared_memory_ = environment.SharedMemoryName();
  host_name_ = environment.HostName();
  port_ = environment.Port();
  TransferDataToWindow();
}

bool EnvironmentDialog::GetEnvironment(IEnvironment& environment) {
  TransferDataFromWindow();
  bool modified = false;
  if (environment.Name() != name_.ToStdString()) {
    environment.Name(name_.ToStdString());
    modified = true;
  }
  if (environment.Description() != description_.ToStdString()) {
    environment.Description(description_.ToStdString());
    modified = true;
  }
  if (environment.ConfigFile() != config_file_.ToStdString()) {
    environment.ConfigFile(config_file_.ToStdString());
    modified = true;
  }
  if (environment.SharedMemoryName() != shared_memory_.ToStdString()) {
    environment.SharedMemoryName(shared_memory_.ToStdString());
    modified = true;
  }
  if (environment.HostName() != host_name_.ToStdString()) {
    environment.HostName(host_name_.ToStdString());
    modified = true;
  }
  if (environment.Port() != port_) {
    environment.Port(port_);
  }
  return modified;
}

void EnvironmentDialog::OnSave(wxCommandEvent &event) {
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

void EnvironmentDialog::OnUpdateSave(wxUpdateUIEvent &event) {
  event.Enable(!name_.IsEmpty());
}

void EnvironmentDialog::OnConfigPicker(wxFileDirPickerEvent& event) {
  const wxString file = event.GetPath();
  try {
    std::filesystem::path full_name(file.ToStdWstring());
    if (auto* config = wxConfig::Get();
        config != nullptr) {
      config->Write("/EnvironmentDialog/Path",
                    wxString(full_name.parent_path().wstring()));
    }
  } catch (const std::exception&) {
  }
}

void EnvironmentDialog::OnNameChange(wxCommandEvent&) {
  name_ctrl_->TransferDataFromWindow();
}

bool EnvironmentDialog::TransferDataToWindow() {
  std::wostringstream app_arg;
  config_picker_->SetPath(config_file_);
  return wxWindowBase::TransferDataToWindow();
}

bool EnvironmentDialog::TransferDataFromWindow() {  // Fix the exe_picker and combo boxes
  config_file_ = config_picker_->GetPath();
  auto ret = wxWindowBase::TransferDataFromWindow();
  name_.Trim(true).Trim(false);
  description_.Trim(true).Trim(false);
  config_file_.Trim(true).Trim(false);
  shared_memory_.Trim(true).Trim(false);
  host_name_.Trim(true).Trim(false);
  return ret;
}

bool EnvironmentDialog::IsValidName() const {
  const std::string ref_name = name_.ToStdString();
  const bool valid = std::ranges::none_of( invalid_names_,
                                       [&] (const std::string& name) -> bool {
                                            return IEquals(name, ref_name);
                                       });
  return valid;
}

} // bus
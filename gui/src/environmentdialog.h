/*
* Copyright 2025 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <vector>
#include <string>

#include <wx/wx.h>
#include <wx/filepicker.h>
#include "bus/ienvironment.h"

namespace bus {

class EnvironmentDialog : public wxDialog {
 public:
  EnvironmentDialog(wxWindow *parent);
  void SetInvalidNames(std::vector<std::string>& invalid_names);
  void SetEnvironment(const IEnvironment& environment);
  bool GetEnvironment(IEnvironment& environment);
  bool TransferDataToWindow() override;
  bool TransferDataFromWindow() override;
 private:

  wxString name_;
  wxString description_;
  wxString config_file_;
  wxString shared_memory_;
  wxString host_name_ = "!27.0.0.1";
  uint16_t port_ = 43611;

  wxFilePickerCtrl* config_picker_ = nullptr;
  wxTextCtrl* name_ctrl_ = nullptr;
  std::vector<std::string> invalid_names_;

  [[nodiscard]] bool IsValidName() const;

  void OnUpdateSave(wxUpdateUIEvent &event);
  void OnSave(wxCommandEvent& event);
  void OnConfigPicker(wxFileDirPickerEvent& event);
  void OnNameChange(wxCommandEvent& event);
  wxDECLARE_EVENT_TABLE();
};

} // bus


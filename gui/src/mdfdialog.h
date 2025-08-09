/*
* Copyright 2025 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <vector>
#include <string>

#include <wx/wx.h>
#include <wx/filepicker.h>
#include "bus/isource.h"

namespace bus {

class MdfDialog : public wxDialog {
 public:
  MdfDialog(wxWindow *parent);
  void SetInvalidNames(std::vector<std::string>& invalid_names);
  void SetSource(const ISource& source);
  bool GetSource(ISource& source);
  bool TransferDataToWindow() override;
  bool TransferDataFromWindow() override;
 private:

  wxString name_;
  wxString description_;
  wxString filename_;

  wxFilePickerCtrl* file_picker_ = nullptr;
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




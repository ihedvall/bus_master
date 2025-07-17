/*
* Copyright 2025 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */

#pragma once


#include <wx/wx.h>
#include <wx/filepicker.h>
#include "bus/project.h"

namespace bus {

class ProjectDialog : public wxDialog {
public:
  ProjectDialog(wxWindow *parent);
  void SetProject(const Project& project);
  /**
   * @brief Updates the project properties.
   * @param project Reference to a project object.
   * @return True if the project object was modified.
   */
  bool GetProject(Project& project);

  bool TransferDataToWindow() override;
  bool TransferDataFromWindow() override;
  private:

  wxString name_;
  wxString description_;
  wxString config_file_;
  wxFilePickerCtrl* config_picker_ = nullptr;
  wxTextCtrl* name_ctrl_ = nullptr;
  void OnUpdateSave(wxUpdateUIEvent &event);
  void OnSave(wxCommandEvent& event);
  void OnConfigPicker(wxFileDirPickerEvent& event);
  void OnNameChange(wxCommandEvent& event);
  wxDECLARE_EVENT_TABLE();
};

} // bus


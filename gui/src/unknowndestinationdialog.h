/*
* Copyright 2025 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <vector>
#include <string>

#include <wx/wx.h>

#include "bus/idestination.h"

namespace bus {

class UnknownDestinationDialog : public wxDialog {
 public:
  UnknownDestinationDialog(wxWindow *parent);

  void SetInvalidNames(std::vector<std::string>& invalid_names);

  void SetDestination(const IDestination& destination);
  bool GetDestination(IDestination& destination);

  bool TransferDataToWindow() override;
  bool TransferDataFromWindow() override;
 private:

  wxString name_;
  wxString description_;

  wxTextCtrl* name_ctrl_ = nullptr;
  std::vector<std::string> invalid_names_;

  [[nodiscard]] bool IsValidName() const;

  void OnUpdateSave(wxUpdateUIEvent &event);
  void OnSave(wxCommandEvent& event);
  void OnNameChange(wxCommandEvent& event);

  wxDECLARE_EVENT_TABLE();
};

} // bus

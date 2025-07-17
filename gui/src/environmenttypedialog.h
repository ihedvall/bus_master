/*
 * Copyright 2025 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */

#pragma once
#include <wx/wx.h>
#include <wx/choice.h>

#include "bus/ienvironment.h"

namespace bus {

class EnvironmentTypeDialog : public wxDialog {
 public:
  EnvironmentTypeDialog(wxWindow *parent);
  [[nodiscard]] TypeOfEnvironment GetType() const;
 protected:

 private:
  wxString type_ = std::string(IEnvironment::TypeToString(
      TypeOfEnvironment::SuperviseMasterEnvironment));
  wxChoice* type_ctrl_ = nullptr;
  void OnUpdateSave(wxUpdateUIEvent &event);
  void OnSave(wxCommandEvent& event);
  wxDECLARE_EVENT_TABLE();
};


}  // namespace bus



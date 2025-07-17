/*
* Copyright 2025 Ingemar Hedvall
* SPDX-License-Identifier: MIT
*/

#pragma once
#include <wx/wx.h>
#include <wx/choice.h>

#include "bus/idatabase.h"

namespace bus {

class DatabaseTypeDialog : public wxDialog {
public:
 DatabaseTypeDialog(wxWindow *parent);
 [[nodiscard]] TypeOfDatabase GetType() const;
protected:

private:
 wxString type_ = std::string(IDatabase::TypeToString(
     TypeOfDatabase::Unknown));
 wxChoice* type_ctrl_ = nullptr;
 void OnUpdateSave(wxUpdateUIEvent &event);
 void OnSave(wxCommandEvent& event);
 wxDECLARE_EVENT_TABLE();
};


}  // namespace bus


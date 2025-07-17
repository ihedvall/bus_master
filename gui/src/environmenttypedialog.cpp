/*
* Copyright 2025 Ingemar Hedvall
* SPDX-License-Identifier: MIT
*/

#include "environmenttypedialog.h"

#include <wx/valgen.h>

#include "bus/ienvironment.h"

namespace {
  const wxArrayString kEnvTypes = {
    wxString(bus::IEnvironment::TypeToString(bus::TypeOfEnvironment::DummyEnvironment)),
    wxString(bus::IEnvironment::TypeToString(bus::TypeOfEnvironment::SuperviseMasterEnvironment)),
  };
}

namespace bus {

wxBEGIN_EVENT_TABLE(EnvironmentTypeDialog, wxDialog) //NOLINT
    EVT_UPDATE_UI(wxID_SAVE,EnvironmentTypeDialog::OnUpdateSave)
    EVT_BUTTON(wxID_SAVE, EnvironmentTypeDialog::OnSave)
 wxEND_EVENT_TABLE()

EnvironmentTypeDialog::EnvironmentTypeDialog(wxWindow *parent)
    : wxDialog(parent,wxID_ANY,"Select the Environment Type",wxDefaultPosition,
               wxDefaultSize) {
  auto* save_button = new wxButton(this, wxID_SAVE, wxGetStockLabel(wxID_SAVE));
  auto* cancel_button = new wxButton(this, wxID_CANCEL, wxGetStockLabel(wxID_CANCEL));

  type_ctrl_ = new wxChoice(this, wxID_ANY, wxDefaultPosition,wxDefaultSize,
                            kEnvTypes, wxCB_SORT,
                            wxGenericValidator(&type_));;
  type_ctrl_->SetMinSize({20*8,-1});


  auto* type_label = new wxStaticText(this, wxID_ANY, L"Environment Type:");

  int label_width = 100;
  label_width = std::max(label_width,type_label->GetBestSize().GetX());

  auto* type_sizer = new wxBoxSizer(wxHORIZONTAL);
  type_label->SetMinSize({label_width, -1});
  type_sizer->Add(type_label, 0, wxALIGN_CENTER_VERTICAL | wxLEFT, 5);
  type_sizer->Add(type_ctrl_, 0, wxALIGN_CENTER_VERTICAL | wxLEFT | wxRIGHT, 5);

  auto* system_sizer = new wxStdDialogButtonSizer();
  system_sizer->AddButton(save_button);
  system_sizer->AddButton(cancel_button);
  system_sizer->Realize();


  auto* main_sizer = new wxBoxSizer(wxVERTICAL);
  main_sizer->Add(type_sizer, 0, wxALIGN_LEFT | wxTOP | wxBOTTOM | wxEXPAND, 4);
  main_sizer->Add(system_sizer, 0, wxALIGN_CENTER_HORIZONTAL | wxBOTTOM | wxLEFT | wxRIGHT, 10);

  SetSizerAndFit(main_sizer);
  save_button->SetDefault();
}

TypeOfEnvironment EnvironmentTypeDialog::GetType() const {

  return IEnvironment::TypeFromString(type_.ToStdString());
}

void EnvironmentTypeDialog::OnSave(wxCommandEvent &event) {
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

void EnvironmentTypeDialog::OnUpdateSave(wxUpdateUIEvent &event) {
  event.Enable(true);
}

}  // namespace bus

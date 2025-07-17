/*
 * Copyright 2025 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */

#include "projectframe.h"

#include <wx/panel.h>
#include <wx/sizer.h>

#include <filesystem>
#include <sstream>

#include "busmaster.h"
#include "environmentview.h"
#include "databaseview.h"
#include "projectdialog.h"
#include "projectdocument.h"
#include "projecttree.h"
#include "projectview.h"
#include "propertyview.h"
#include "windowid.h"

namespace bus {

wxIMPLEMENT_DYNAMIC_CLASS(ProjectFrame,wxPanel)

wxBEGIN_EVENT_TABLE(ProjectFrame, wxPanel)


wxEND_EVENT_TABLE()

ProjectFrame::ProjectFrame(wxDocParentFrame *parent)
    : wxPanel( parent ) {

  splitter_ = new wxSplitterWindow(this, wxID_ANY,
    wxDefaultPosition, wxDefaultSize,
    wxSP_3D | wxCLIP_CHILDREN);
  project_panel_ = new ProjectTree(splitter_);
  property_view_ = new PropertyView(splitter_);

  environment_view_ = new EnvironmentView(splitter_);
  environment_view_->Hide();

  database_view_ = new DatabaseView(splitter_);
  database_view_->Hide();

  splitter_->SetSashGravity(0.0);
  splitter_->SplitVertically(project_panel_, property_view_ , 300);

  auto* main_sizer = new wxBoxSizer(wxVERTICAL);
  main_sizer->Add(splitter_, 1, wxEXPAND | wxLEFT | wxBOTTOM, 0);

  SetSizerAndFit(main_sizer);
}

void ProjectFrame::Update() {
  if (GetDocument() == nullptr) {
    return;
  }
  RedrawLeft();
  RedrawRight();
}

ProjectDocument *ProjectFrame::GetDocument() const {
  return view_ != nullptr ? view_->GetDoc() : nullptr;
}

void ProjectFrame::RedrawLeft() {
  project_panel_->Update();
}

void ProjectFrame::RedrawRight() {
  const auto* doc = GetDocument();
  if (doc == nullptr || splitter_ == nullptr) {
    return;
  }

  const ProjectItemType type = doc->GetCurrentType();
  wxWindow* new_view = property_view_;

  switch (type) {
    case ProjectItemType::Environments:
      new_view = environment_view_ ;
      break;

    case ProjectItemType::Databases:
      new_view = database_view_ ;
      break;

    default:
      break;
  }
  wxWindow* current_view = splitter_->GetWindow2();
  if (current_view == nullptr || new_view == nullptr) {
    return;
  }
  if (current_view != new_view) {
    current_view->Hide();
    splitter_->ReplaceWindow(current_view, new_view);
    if (new_view != nullptr) {
      new_view->Show();
      new_view->Update();
    }
  } else {
    current_view->Update();
  }
}

void ProjectFrame::SetView(ProjectView *view) {
  view_ = view;
  if (project_panel_ != nullptr) {
    project_panel_->SetView(view);
  }
  if (property_view_ != nullptr) {
    property_view_->SetView(view);
  }
  if (environment_view_ != nullptr) {
    environment_view_->SetView(view);
  }
  if (database_view_ != nullptr) {
    database_view_->SetView(view);
  }
}

} // end namespace bus




/*
 * Copyright 2025 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */
#pragma once
#include <wx/imaglist.h>
#include <wx/listctrl.h>
#include <wx/panel.h>
#include <wx/splitter.h>
#include <wx/wx.h>

#include "projectdocument.h"

namespace bus {

class ProjectView;
class ProjectTree;
class PropertyView;
class EnvironmentView;
class DatabaseView;

class ProjectFrame : public wxPanel {
 public:
  ProjectFrame() = default;
  explicit ProjectFrame(wxDocParentFrame *parent);

  void Update() override;

  void SetView(ProjectView* view);

  [[nodiscard]] ProjectDocument* GetDocument() const;

  void RedrawLeft();
  void RedrawRight();

  private:
  ProjectView* view_ = nullptr;

  ProjectTree* project_panel_ = nullptr;
  PropertyView* property_view_ = nullptr;
  EnvironmentView* environment_view_ = nullptr;
  DatabaseView* database_view_ = nullptr;
  wxSplitterWindow* splitter_ = nullptr;
  wxDECLARE_DYNAMIC_CLASS(ProjectFrame);
  wxDECLARE_EVENT_TABLE();

};
}





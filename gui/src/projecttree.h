/*
 * Copyright 2025 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */

#pragma once
#include <wx/wx.h>
#include <wx/splitter.h>
#include <wx/panel.h>
#include <wx/treectrl.h>
#include <wx/bitmap.h>


namespace bus {
class ProjectView;
class ProjectDocument;
class Project;

class ProjectTree : public wxPanel {
 public:
  explicit ProjectTree(wxSplitterWindow *parent);

  void SetView(ProjectView* view) { view_ = view; }

  [[nodiscard]] ProjectDocument* GetDocument();
  [[nodiscard]] Project* GetProject();
  void Update() override;
 private:
  ProjectView* view_ = nullptr;
  wxTreeCtrl* tree_ = nullptr;
  wxImageList image_list_;

  wxTreeItemId FindCurrent(wxTreeItemId root_item);

  void Redraw();
  void RedrawEnvironments(wxTreeItemId& root_item);
  void RedrawDatabases(wxTreeItemId& root_item);
  void RedrawSources(wxTreeItemId& root_item);
  void RedrawDestinations(wxTreeItemId& root_item);
  void RedrawRight();

  void OnTreeSelected(wxTreeEvent& event);
  void OnTreeRightClick(wxTreeEvent& event);

  wxDECLARE_EVENT_TABLE();
};

} // bus


/*
* Copyright 2025 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <wx/wx.h>
#include <wx/splitter.h>
#include <wx/panel.h>
#include <wx/listctrl.h>

namespace bus {
class ProjectView;
class ProjectDocument;
class Project;

class SourceView : public wxPanel {
 public:
  explicit SourceView(wxSplitterWindow* parent);

  void SetView(ProjectView* view) { view_ = view; }

  [[nodiscard]] ProjectDocument* GetDocument() const;
  [[nodiscard]] Project* GetProject() const;
  void Update() override;

 private:
  ProjectView* view_ = nullptr;
  wxListView* list_ = nullptr;
  wxImageList image_list_;

  void Redraw();

  void OnRightClick(wxListEvent& event);
  void OnItemSelected(wxListEvent& event);
  wxDECLARE_EVENT_TABLE();
};

} // end namespace bus
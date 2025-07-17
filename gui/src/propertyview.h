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

class PropertyView : public wxPanel {
public:
  explicit PropertyView(wxSplitterWindow *parent);

  void SetView(ProjectView* view) { view_ = view; }

  [[nodiscard]] ProjectDocument* GetDocument();
  void Update() override;

  void Redraw();
private:
  ProjectView* view_ = nullptr;
  wxListView* list_ = nullptr;
  void OnRightClick(wxListEvent& event);
  wxDECLARE_EVENT_TABLE();
};

} // bus


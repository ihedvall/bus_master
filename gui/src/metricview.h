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
class IDatabase;

class MetricView : public wxPanel {
 public:
  explicit MetricView(wxSplitterWindow* parent);

  void SetView(ProjectView* view) { view_ = view; }

  [[nodiscard]] ProjectDocument* GetDocument() const;
  [[nodiscard]] IDatabase* GetDatabase() const;
  void Update() override;

 private:
  ProjectView* view_ = nullptr;
  wxListView* list_ = nullptr;
  wxStaticText* header_ctrl_ = nullptr;
  wxTextCtrl* filter_name_ctrl_ = nullptr;
  wxTextCtrl* filter_group_ctrl_ = nullptr;
  //wxImageList image_list_;

  void Redraw();

  wxString MakeHeaderText();

  void OnRightClick(wxListEvent& event);
  void OnItemSelected(wxListEvent& event);
  wxDECLARE_EVENT_TABLE();
};

} // end namespace bus
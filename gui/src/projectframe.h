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
class MetricView;
class LogListView;
class MessageListView;
class SourceView;
class DestinationView;
class ProjectFrame : public wxPanel {
 public:
  ProjectFrame() = default;
  explicit ProjectFrame(wxDocParentFrame *parent);

  void Update() override;

  void SetView(ProjectView* view);

  [[nodiscard]] ProjectDocument* GetDocument() const;

  void RedrawLeft();
  void RedrawRight();

  void CheckLogView();
  [[nodiscard]] bool IsLogViewVisible() const;
  void ShowLogView(bool show);
 private:
  ProjectView* view_ = nullptr;
  ProjectTree* project_panel_ = nullptr;
  PropertyView* property_view_ = nullptr;
  EnvironmentView* environment_view_ = nullptr;
  DatabaseView* database_view_ = nullptr;
  MetricView* metric_view_ = nullptr;
  wxSplitterWindow* splitter_ = nullptr;
  LogListView* log_view_ = nullptr;
  MessageListView* message_view_ = nullptr;
  SourceView* source_view_ = nullptr;
  DestinationView* destination_view_ = nullptr;

  wxDECLARE_DYNAMIC_CLASS(ProjectFrame);
  wxDECLARE_EVENT_TABLE();

};
}





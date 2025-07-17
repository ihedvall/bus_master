/*
 * Copyright 2025 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */
#pragma once
#include <wx/docview.h>


namespace bus {
class Project;
class ProjectFrame;
class MainFrame;
class ProjectDocument;
class ProjectView : public wxView  {
 public:
  ProjectView() = default;
  [[nodiscard]] ProjectDocument* GetDoc() const;
  [[nodiscard]] Project* GetProject() const;
  [[nodiscard]] ProjectFrame* GetProjectFrame() const;
  [[nodiscard]] MainFrame* GetMainFrame() const;

  bool OnCreate(wxDocument* doc, long flags) override;
  bool OnClose(bool del) override;

  void OnDraw(wxDC *dc) override;
  void OnUpdate(wxView *sender, wxObject *hint) override;

  void OnUpdateProjectExist(wxUpdateUIEvent& event);


 private:
  void UpdateTitle();
  wxDECLARE_DYNAMIC_CLASS(ProjectView);
  wxDECLARE_EVENT_TABLE();
 };

}






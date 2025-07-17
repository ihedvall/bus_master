/*
 * Copyright 2025 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */
#pragma once

#include <wx/wx.h>
#include <wx/docview.h>
#include <wx/timer.h>

namespace bus {

class ProjectDocument;
class ProjectFrame;
class MainFrame : public wxDocParentFrame {
 public:
  MainFrame(wxDocManager* doc_manager,
            const wxString& title,
            const wxPoint& start_pos,
            const wxSize& start_size,
            bool maximized);

  ProjectFrame* GetFrame() { return project_frame_; }
  ProjectDocument* GetDocument() const;

 private:

  ProjectFrame* project_frame_;
  wxStatusBar* status_bar_ = nullptr;
  wxTimer* timer_ = nullptr;


  void OnClose(wxCloseEvent& event);
  void OnAbout(wxCommandEvent& event);

  void OnDropFiles(wxDropFilesEvent& event);

  void OnUpdateNoDocument(wxUpdateUIEvent& event);
  void OnTimer(wxTimerEvent& event);

  wxDECLARE_EVENT_TABLE();
};

} // end namespace bus


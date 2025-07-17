/*
 * Copyright 2025 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */
#include "projectview.h"

#include <util/logstream.h>
#include <wx/docview.h>

#include <filesystem>
#include <sstream>

#include "busmaster.h"
#include "mainframe.h"
#include "projectdocument.h"
#include "projectframe.h"
#include "windowid.h"

using namespace std::filesystem;
using namespace util::log;

namespace bus {

wxIMPLEMENT_DYNAMIC_CLASS(ProjectView,wxView)

wxBEGIN_EVENT_TABLE(ProjectView, wxView)

wxEND_EVENT_TABLE()

ProjectDocument *ProjectView::GetDoc() const {
  return dynamic_cast<ProjectDocument*>(wxView::GetDocument());
}

Project *ProjectView::GetProject() const {
  if (auto* doc = GetDoc(); doc != nullptr) {
    return doc->GetProject();
  }
  return nullptr;
}

ProjectFrame* ProjectView::GetProjectFrame() const {
  if (auto* parent = GetMainFrame();
      parent != nullptr ) {
    return parent->GetFrame();
  }
  return nullptr;
}

MainFrame* ProjectView::GetMainFrame() const {
  const auto& app = wxGetApp();
  return dynamic_cast<MainFrame*>(app.GetTopWindow());
}

void ProjectView::OnDraw(wxDC* dc) {

}

bool ProjectView::OnCreate(wxDocument *doc, long flags) {
  if (!wxView::OnCreate( doc,flags)) {
    return false;
  }

  if (auto* frame = GetProjectFrame();
      frame != nullptr) {
    frame->SetView(this);
  }
  UpdateTitle();
  return true;
}

bool ProjectView::OnClose(bool del) {
  if (!wxView::OnClose(del)) {
    return false;
  }
  Activate(false);

  auto& app = wxGetApp();
  if (auto* parent = wxDynamicCast(app.GetTopWindow(),MainFrame);
      parent != nullptr ) {
    if (auto* frame = parent->GetFrame();
        frame != nullptr) {
      frame->ClearBackground();
      frame->SetView(nullptr);
    }
    parent->SetTitle(app.GetAppDisplayName());
  }
  SetFrame(nullptr);
  return true;
}

void ProjectView::OnUpdate(wxView* sender, wxObject* hint) {
  wxView::OnUpdate(sender, hint);
  UpdateTitle();
  if (auto* frame = GetProjectFrame(); frame != nullptr) {
    frame->Update();
  }
}

void ProjectView::OnUpdateProjectExist(wxUpdateUIEvent& event) {
  if (const auto* project = GetProject(); project != nullptr) {
    event.Enable(true);
  } else {
    event.Enable(false);
  }
}

void ProjectView::UpdateTitle() {
  const auto& app = wxGetApp();
  std::wostringstream title;
  title << app.GetAppDisplayName();
  if (const auto* doc = GetDoc(); doc != nullptr) {
    wxString name = doc->GetFilename();
    std::wstring short_name;
    try {
      const path fullname(name.ToStdWstring());
      const path filename = fullname.filename();
      short_name = filename.wstring();
    } catch (const std::exception err) {
      LOG_ERROR() << "Filename error. Error: " << err.what();
      short_name.clear();
    }
    if (short_name.empty()) {
      title << " - " << "No Name";
    } else {
      title << " - " << short_name;
    }
  }
  if (auto* main_frame = GetMainFrame(); main_frame != nullptr) {
    const wxString old_title = main_frame->GetTitle();
    if (!old_title.IsSameAs(title.str()) ) {
      main_frame->SetTitle(title.str());
    }
  }
}


}  // namespace bus
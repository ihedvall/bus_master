/*
* Copyright 2025 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */
#include "databaseview.h"

#include <sstream>

#include "bus/project.h"
#include "projectdocument.h"
#include "projectview.h"
#include "windowid.h"

namespace bus {
wxBEGIN_EVENT_TABLE(DatabaseView, wxPanel)
    EVT_LIST_ITEM_RIGHT_CLICK(kIdDatabaseList, DatabaseView::OnRightClick)
wxEND_EVENT_TABLE()

DatabaseView::DatabaseView(wxSplitterWindow* parent)
    : wxPanel(parent, kIdDatabasePanel) {
  auto* sizer = new wxBoxSizer(wxVERTICAL);
  list_ = new wxListView(this, kIdDatabaseList,
                         wxDefaultPosition, wxDefaultSize,
                         wxLC_REPORT | wxLC_SINGLE_SEL);
  list_->AppendColumn("State", wxLIST_FORMAT_LEFT, 50);
  list_->AppendColumn("Name", wxLIST_FORMAT_LEFT, 150);
  list_->AppendColumn("Status", wxLIST_FORMAT_LEFT, 100);
  list_->AppendColumn("Description", wxLIST_FORMAT_LEFT, 400);
  sizer->Add(list_, 1, wxALL | wxEXPAND, 0);
  SetSizer(sizer);
}

ProjectDocument *DatabaseView::GetDocument() const {
  return view_ != nullptr ? view_->GetDoc() : nullptr;
}

Project *DatabaseView::GetProject() const {
  if (const auto* doc = GetDocument(); doc != nullptr) {
    return doc->GetProject();
  }
  return nullptr;
}

void DatabaseView::Update() {
  if (GetDocument() == nullptr) {
    return;
  }
  Redraw();
}

void DatabaseView::Redraw() {
  if (!IsShown()) {
    return;
  }

  list_->DeleteAllItems();
  Project* project = GetProject();
  if (project == nullptr) {
    return;
  }

  long line = 0;
  for (const auto& db : project->Databases()) {
    if (!db) {
      continue;
    }
    // ToDo: Fix Bitmap
    std::string status;
    if (db->IsActive() && db->IsOperable()) {
      status = "Active";
    } else if (db->IsActive() && !db->IsOperable()) {
      status = "Failing";
    } else if (!db->IsActive() && db->IsOperable()) {
      status = "Stopping";
    } else {
      status = "Inactive";
    }
    const auto index = list_->InsertItem(line, "");
    list_->SetItem(index, 1, wxString::FromUTF8(db->Name()));
    list_->SetItem(index, 2, wxString::FromUTF8(status));
    list_->SetItem(index, 3, wxString::FromUTF8(db->Description()));
    ++line;
  }

}

void DatabaseView::OnRightClick(wxListEvent& event) {
  /* ToDo: Fix right-click
  auto* doc = GetDocument();
  if (doc == nullptr) {
    return;
  }
  auto* project = doc->GetProject();;
  if (project == nullptr) {
    return;
  }
  const auto selected_item = event.GetIndex();
  if (selected_item < 0) {
    return;
  }

  switch (doc->GetCurrentType()) {
    case ProjectItemType::Environment:
      if (auto* env = project->GetEnvironment(doc->GetCurrentId());
          env != nullptr ) {
        wxMenu menu("Environment");
        menu.Append(kIdAddEnvironment, wxGetStockLabel(wxID_ADD),
                    "Add a new environment");
        menu.Append(kIdEditEnvironment, wxGetStockLabel(wxID_EDIT),
                    "Change the environment configuration");
        menu.Append(kIdDeleteEnvironment, wxGetStockLabel(wxID_DELETE),
                    "Delete the environment configuration");
        menu.AppendSeparator();
        menu.Append(kIdStartEnvironment, "Start", "Start Environment");
        menu.Append(kIdStopEnvironment, "Stop", "Start Environment");
        PopupMenu(&menu, event.GetPoint());
      }
      break;


    default: // Show nothing
      break;
  }
   */
}

}  // namespace bus
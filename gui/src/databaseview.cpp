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

namespace {
constexpr int kDbActiveBmp = 0;
constexpr int kDbDeactiveBmp = 1;
constexpr int kDbRunningBmp = 2;
constexpr int kDbFailingBmp = 3;
constexpr int kDbStoppedBmp = 4;
}

namespace bus {
wxBEGIN_EVENT_TABLE(DatabaseView, wxPanel)
    EVT_LIST_ITEM_SELECTED(kIdDatabaseList, DatabaseView::OnItemSelected)
    EVT_LIST_ITEM_RIGHT_CLICK(kIdDatabaseList, DatabaseView::OnRightClick)
wxEND_EVENT_TABLE()

DatabaseView::DatabaseView(wxSplitterWindow* parent)
    : wxPanel(parent, kIdDatabasePanel),
      image_list_(16,16,false,5) {

  wxBitmap list_view("LIST_VIEW", wxBITMAP_TYPE_BMP_RESOURCE);

  auto* sizer = new wxBoxSizer(wxVERTICAL);
  list_ = new wxListView(this, kIdDatabaseList,
                         wxDefaultPosition, wxDefaultSize,
                         wxLC_REPORT | wxLC_SINGLE_SEL);
  list_->AppendColumn("Enabled", wxLIST_FORMAT_LEFT, 100);
  list_->AppendColumn("Name", wxLIST_FORMAT_LEFT, 150);
  list_->AppendColumn("Type", wxLIST_FORMAT_LEFT, 100);
  list_->AppendColumn("Status", wxLIST_FORMAT_LEFT, 100);
  list_->AppendColumn("Description", wxLIST_FORMAT_LEFT, 400);
  sizer->Add(list_, 1, wxALL | wxEXPAND, 0);
  SetSizer(sizer);

  image_list_.Add(list_view);
  list_->SetImageList(&image_list_, wxIMAGE_LIST_SMALL);
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
    std::string status;
    int status_bmp = kDbStoppedBmp;
    const std::string_view type = IDatabase::TypeToString(db->Type());

    if (db->IsEnabled() && db->IsOperable()) {
      status = "Enabled";
      status_bmp = kDbRunningBmp;
    } else if (db->IsEnabled() && !db->IsOperable()) {
      status = "Failing";
      status_bmp = kDbFailingBmp;
    } else if (!db->IsEnabled() && db->IsOperable()) {
      status = "Stopping";
    } else {
      status = "Disabled";
    }
    const auto index = list_->InsertItem(line, "",
           db->IsEnabled() ? kDbActiveBmp : kDbDeactiveBmp);
    list_->SetItem(index, 1, wxString::FromUTF8(db->Name()));
    list_->SetItem(index, 2, wxString::FromUTF8(type));
    list_->SetItem(index, 3, wxString::FromUTF8(status), status_bmp);
    list_->SetItem(index, 4, wxString::FromUTF8(db->Description()));
    ++line;
  }

}

void DatabaseView::OnRightClick(wxListEvent& event) {
  const auto* doc = GetDocument();
  if (doc == nullptr) {
    return;
  }
  const auto* db = doc->GetCurrentDatabase();
  if (db == nullptr) {
    return;
  }
  wxMenu menu("Database");
  menu.Append(kIdEditDatabase, wxGetStockLabel(wxID_EDIT),
              "Change the database configuration");
  menu.Append(kIdDeleteDatabase, wxGetStockLabel(wxID_DELETE),
              "Delete the database configuration");
  menu.AppendSeparator();
  menu.Append(kIdActivateDatabase, "Enable",
              "Enable the database.");
  menu.Append(kIdDeactivateDatabase, "Disable",
              "Disable the database.");

  PopupMenu(&menu, event.GetPoint());

}

void DatabaseView::OnItemSelected(wxListEvent& event) {
  const wxListItem& current_item = event.GetItem();
  wxString name = list_->GetItemText(current_item, 1);
  if (auto* doc = GetDocument();doc != nullptr) {
    doc->SetCurrentItem(ProjectItemType::Databases, name.ToStdString());
  }

}

}  // namespace bus
/*
* Copyright 2025 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */
#include "destinationview.h"

#include <sstream>

#include "bus/project.h"
#include "projectdocument.h"
#include "projectview.h"
#include "windowid.h"

namespace {
constexpr int kDestinationEnabledBmp = 0;
constexpr int kDestinationDisabledBmp = 1;
constexpr int kDestinationRunningBmp = 2;
constexpr int kDestinationFailingBmp = 3;
constexpr int kDestinationStoppedBmp = 4;
}

namespace bus {
wxBEGIN_EVENT_TABLE(DestinationView, wxPanel)
    EVT_LIST_ITEM_SELECTED(kIdDestinationList, DestinationView::OnItemSelected)
    EVT_LIST_ITEM_RIGHT_CLICK(kIdDestinationList, DestinationView::OnRightClick)
wxEND_EVENT_TABLE()

DestinationView::DestinationView(wxSplitterWindow* parent)
    : wxPanel(parent, kIdDatabasePanel),
      image_list_(16,16,false,5) {
  wxBitmap list_view("LIST_VIEW", wxBITMAP_TYPE_BMP_RESOURCE);

  auto* sizer = new wxBoxSizer(wxVERTICAL);
  list_ = new wxListView(this, kIdDestinationList,
                         wxDefaultPosition, wxDefaultSize,
                         wxLC_REPORT | wxLC_SINGLE_SEL);
  list_->AppendColumn("Enabled", wxLIST_FORMAT_LEFT, 75);
  list_->AppendColumn("Name", wxLIST_FORMAT_LEFT, 150);
  list_->AppendColumn("Type", wxLIST_FORMAT_LEFT, 150);
  list_->AppendColumn("Status", wxLIST_FORMAT_LEFT, 100);
  list_->AppendColumn("Description", wxLIST_FORMAT_LEFT, 400);
  sizer->Add(list_, 1, wxALL | wxEXPAND, 0);
  SetSizer(sizer);

  image_list_.Add(list_view);
  list_->SetImageList(&image_list_, wxIMAGE_LIST_SMALL);
}

ProjectDocument *DestinationView::GetDocument() const {
  return view_ != nullptr ? view_->GetDoc() : nullptr;
}

Project *DestinationView::GetProject() const {
  if (const auto* doc = GetDocument(); doc != nullptr) {
    return doc->GetProject();
  }
  return nullptr;
}

void DestinationView::Update() {
  if (GetDocument() == nullptr) {
    return;
  }
  Redraw();
}

void DestinationView::Redraw() {
  if (!IsShown()) {
    return;
  }

  list_->DeleteAllItems();
  Project* project = GetProject();
  if (project == nullptr) {
    return;
  }

  long line = 0;
  for (const auto& destination : project->Destinations()) {
    if (!destination) {
      continue;
    }
    std::string status;
    int enabled_bmp = kDestinationDisabledBmp;
    int status_bmp = kDestinationStoppedBmp;
    const std::string_view type = IDestination::TypeToString(destination->Type());

    if (destination->IsEnabled()) {
      enabled_bmp = kDestinationEnabledBmp;
    }
    if (destination->IsStarted() && destination->IsOperable()) {
      status = "Running";
      status_bmp = kDestinationRunningBmp;
    } else if (destination->IsEnabled() && !destination->IsOperable()) {
      status = "Failing";
      status_bmp = kDestinationFailingBmp;
    } else if (!destination->IsEnabled() && destination->IsOperable()) {
      status = "Stopping";
    } else {
      status = "Stopped";
    }
    const auto index = list_->InsertItem(line, "", enabled_bmp);
    list_->SetItem(index, 1, wxString::FromUTF8(destination->Name()));
    list_->SetItem(index, 2, wxString::FromUTF8(type));
    list_->SetItem(index, 3, wxString::FromUTF8(status), status_bmp);
    list_->SetItem(index, 4, wxString::FromUTF8(destination->Description()));
    ++line;
  }

}

void DestinationView::OnRightClick(wxListEvent& event) {
  const auto* doc = GetDocument();
  if (doc == nullptr) {
    return;
  }
  const auto* db = doc->GetCurrentDatabase();
  if (db == nullptr) {
    return;
  }
  wxMenu menu("Simulator Destination Tasks");
  menu.Append(kIdEditDestination, wxGetStockLabel(wxID_EDIT),
              "Change the destination task configuration");
  menu.Append(kIdDeleteDestination, wxGetStockLabel(wxID_DELETE),
              "Delete the destination task configuration");
  menu.AppendSeparator();
  menu.Append(kIdEnableDestination, "Enable",
              "Enable the destination task.");
  menu.Append(kIdDisableDestination, "Disable",
              "Disable the destination task");

  PopupMenu(&menu, event.GetPoint());

}

void DestinationView::OnItemSelected(wxListEvent& event) {
  const wxListItem& current_item = event.GetItem();
  wxString name = list_->GetItemText(current_item, 1);
  if (auto* doc = GetDocument(); doc != nullptr) {
    doc->SetCurrentItem(ProjectItemType::Destinations, name.ToStdString());
  }
}

}  // namespace bus
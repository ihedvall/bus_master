/*
* Copyright 2025 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */
#include "sourceview.h"

#include <sstream>

#include "bus/project.h"
#include "projectdocument.h"
#include "projectview.h"
#include "windowid.h"

namespace {
constexpr int kSourceEnabledBmp = 0;
constexpr int kSourceDisabledBmp = 1;
constexpr int kSourceRunningBmp = 2;
constexpr int kSourceFailingBmp = 3;
constexpr int kSourceStoppedBmp = 4;
}

namespace bus {
wxBEGIN_EVENT_TABLE(SourceView, wxPanel)
    EVT_LIST_ITEM_SELECTED(kIdSourceList, SourceView::OnItemSelected)
    EVT_LIST_ITEM_RIGHT_CLICK(kIdSourceList, SourceView::OnRightClick)
wxEND_EVENT_TABLE()

SourceView::SourceView(wxSplitterWindow* parent)
    : wxPanel(parent, kIdDatabasePanel),
      image_list_(16,16,false,5) {

  wxBitmap list_view("LIST_VIEW", wxBITMAP_TYPE_BMP_RESOURCE);

  auto* sizer = new wxBoxSizer(wxVERTICAL);
  list_ = new wxListView(this, kIdSourceList,
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

ProjectDocument *SourceView::GetDocument() const {
  return view_ != nullptr ? view_->GetDoc() : nullptr;
}

Project *SourceView::GetProject() const {
  if (const auto* doc = GetDocument(); doc != nullptr) {
    return doc->GetProject();
  }
  return nullptr;
}

void SourceView::Update() {
  if (GetDocument() == nullptr) {
    return;
  }
  Redraw();
}

void SourceView::Redraw() {
  if (!IsShown()) {
    return;
  }

  list_->DeleteAllItems();
  Project* project = GetProject();
  if (project == nullptr) {
    return;
  }

  long line = 0;
  for (const auto& source : project->Sources()) {
    if (!source) {
      continue;
    }
    std::string status;
    int enabled_bmp = kSourceDisabledBmp;
    int status_bmp = kSourceStoppedBmp;
    const std::string_view type = ISource::TypeToString(source->Type());

    if (source->IsEnabled()) {
      enabled_bmp = kSourceEnabledBmp;
    }
    if (source->IsStarted() && source->IsOperable()) {
      status = "Running";
      status_bmp = kSourceRunningBmp;
    } else if (source->IsEnabled() && !source->IsOperable()) {
      status = "Failing";
      status_bmp = kSourceFailingBmp;
    } else if (!source->IsEnabled() && source->IsOperable()) {
      status = "Stopping";
    } else {
      status = "Stopped";
    }
    const auto index = list_->InsertItem(line, "", enabled_bmp);
    list_->SetItem(index, 1, wxString::FromUTF8(source->Name()));
    list_->SetItem(index, 2, wxString::FromUTF8(type));
    list_->SetItem(index, 3, wxString::FromUTF8(status), status_bmp);
    list_->SetItem(index, 4, wxString::FromUTF8(source->Description()));
    ++line;
  }

}

void SourceView::OnRightClick(wxListEvent& event) {
  const auto* doc = GetDocument();
  if (doc == nullptr) {
    return;
  }
  const auto* db = doc->GetCurrentDatabase();
  if (db == nullptr) {
    return;
  }
  wxMenu menu("Simulator Sources");
  menu.Append(kIdEditSource, wxGetStockLabel(wxID_EDIT),
              "Change the source configuration");
  menu.Append(kIdDeleteSource, wxGetStockLabel(wxID_DELETE),
              "Delete the source configuration");
  menu.AppendSeparator();
  menu.Append(kIdEnableSource, "Enable",
              "Enable the source.");
  menu.Append(kIdDisableSource, "Disable",
              "Disable the source.");

  PopupMenu(&menu, event.GetPoint());

}

void SourceView::OnItemSelected(wxListEvent& event) {
  const wxListItem& current_item = event.GetItem();
  wxString name = list_->GetItemText(current_item, 1);
  if (auto* doc = GetDocument(); doc != nullptr) {
    doc->SetCurrentItem(ProjectItemType::Sources, name.ToStdString());
  }

}

}  // namespace bus
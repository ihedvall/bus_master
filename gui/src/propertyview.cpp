/*
* Copyright 2025 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */
#include "propertyview.h"

#include <sstream>

#include "projectdocument.h"
#include "projectview.h"
#include "windowid.h"

namespace bus {
wxBEGIN_EVENT_TABLE(PropertyView, wxPanel)
    EVT_LIST_ITEM_RIGHT_CLICK(kIdPropertyList, PropertyView::OnRightClick)
wxEND_EVENT_TABLE()

PropertyView::PropertyView(wxSplitterWindow* parent)
  : wxPanel(parent, kIdPropertyPanel) {
  auto* sizer = new wxBoxSizer(wxVERTICAL);
  list_ = new wxListView(this, kIdPropertyList,
    wxDefaultPosition, wxDefaultSize,wxLC_REPORT | wxLC_SINGLE_SEL);
  list_->AppendColumn("Property", wxLIST_FORMAT_LEFT, 150);
  list_->AppendColumn("Value", wxLIST_FORMAT_LEFT, 400);
  sizer->Add(list_, 1, wxALL | wxEXPAND, 0);
  SetSizer(sizer);
}

ProjectDocument *PropertyView::GetDocument() {
  return view_ != nullptr ? view_->GetDoc() : nullptr;

}

void PropertyView::Update() {
  if (GetDocument() == nullptr) {
    return;
  }
  Redraw();
}

void PropertyView::Redraw() {
  if (!IsShown()) {
    return;
  }

  list_->DeleteAllItems();
  auto* doc = GetDocument();
  if (doc == nullptr) {
    return;
  }
  const auto* project = doc->GetProject();
  if (project == nullptr) {
    return;
  }

  std::vector<BusProperty> properties;
  switch (doc->GetCurrentType()) {
    case ProjectItemType::Project:
      project->ToProperties(properties);
      break;

    case ProjectItemType::Environment:
      if (const auto* env = project->GetEnvironment(doc->GetCurrentId());
          env != nullptr ) {
        env->ToProperties(properties);
      }
      break;

    case ProjectItemType::Database:
      if (const auto* db = project->GetDatabase(doc->GetCurrentId());
          db != nullptr ) {
        db->ToProperties(properties);
      }
      break;

    case ProjectItemType::Source:
      if (const auto* source = project->GetSource(doc->GetCurrentId());
          source != nullptr ) {
        source->ToProperties(properties);
      }
      break;

    case ProjectItemType::Destination:
      if (const auto* dest = project->GetDestination(doc->GetCurrentId());
          dest != nullptr ) {
        dest->ToProperties(properties);
      }
      break;
    default:
      break;
  }

  long line = 0;
  for (const auto& prop : properties) {
    std::ostringstream prop_label;
    prop_label << prop.Label();
    if (!prop.Unit().empty()) {
      prop_label << " [" << prop.Unit() << "]";
    }
    const wxString label = wxString::FromUTF8(prop_label.str());
    switch (prop.Type()) {
      case BusPropertyType::HeaderItem: {
        const auto index = list_->InsertItem(line, label);
        auto font = list_->GetItemFont(index);
        font.MakeItalic();
        list_->SetItemFont(index, font);
        break;
      }

      case BusPropertyType::BlankItem:  {
        const auto index = list_->InsertItem(line, "");
        list_->SetItem(index, 1, "");
        break;
      }

      default: {
        const auto index = list_->InsertItem(line, label);
        list_->SetItem(index, 1, wxString::FromUTF8(prop.Value()));
        break;
      }

    }
    ++line;
  }
}

void PropertyView::OnRightClick(wxListEvent& event) {
  auto* doc = GetDocument();
  if (doc == nullptr) {
    return;
  }
  auto* project = doc->GetProject();
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

    case ProjectItemType::Database:
      if (const IDatabase* db = project->GetDatabase(doc->GetCurrentId());
          db != nullptr ) {
        wxMenu menu("Database");
        menu.Append(kIdAddDatabase, wxGetStockLabel(wxID_ADD),
                    "Add a new database");
        menu.Append(kIdEditDatabase, wxGetStockLabel(wxID_EDIT),
                    "Change the database configuration");
        menu.Append(kIdDeleteDatabase, wxGetStockLabel(wxID_DELETE),
                    "Delete the database configuration");
        menu.AppendSeparator();
        menu.Append(kIdActivateDatabase, "Activate", "Use this database");
        menu.Append(kIdDeactivateDatabase, "Deactivate",
                    "Don't use this database");
        PopupMenu(&menu, event.GetPoint());
      }
      break;

    default: // Show nothing
      break;
    }
  }

}  // namespace bus
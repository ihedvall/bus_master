/*
* Copyright 2025 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */
#include "metricview.h"

#include <sstream>
#include <filesystem>

#include "bus/idatabase.h"
#include "projectdocument.h"
#include "projectview.h"
#include "windowid.h"

using namespace std::filesystem;

namespace {

constexpr int kDbActiveBmp = 0;
constexpr int kDbDeactiveBmp = 1;
constexpr int kDbRunningBmp = 2;
constexpr int kDbFailingBmp = 3;
constexpr int kDbStoppedBmp = 4;

struct MetricSorted {
  std::string name;
  std::string group;
  bool operator < (const MetricSorted& sorted) const {
    return name == sorted.name ? group < sorted.group : name < sorted.name;
  }
};

}

namespace bus {
wxBEGIN_EVENT_TABLE(MetricView, wxPanel)
  EVT_LIST_ITEM_SELECTED(kIdDatabaseList, MetricView::OnItemSelected)
  EVT_LIST_ITEM_RIGHT_CLICK(kIdDatabaseList, MetricView::OnRightClick)
wxEND_EVENT_TABLE()

 MetricView::MetricView(wxSplitterWindow* parent)
    : wxPanel(parent, kIdDatabasePanel) {


  wxString header_text = MakeHeaderText();
  header_ctrl_ = new wxStaticText(this, wxID_ANY,
                                  header_text,
                                  wxDefaultPosition, wxDefaultSize,
                                  wxALIGN_LEFT);
  wxFont font = header_ctrl_->GetFont();
  font.MakeLarger();
  font.MakeBold();
  header_ctrl_->SetFont(font);

  list_ = new wxListView(this, kIdDatabaseList,
                         wxDefaultPosition, wxDefaultSize,
                         wxLC_REPORT | wxLC_SINGLE_SEL);
  list_->AppendColumn("Name", wxLIST_FORMAT_LEFT, 200);
  list_->AppendColumn("Group", wxLIST_FORMAT_LEFT, 200);
  list_->AppendColumn("Value", wxLIST_FORMAT_LEFT, 100);
  list_->AppendColumn("Unit", wxLIST_FORMAT_LEFT, 50);
  list_->AppendColumn("Type", wxLIST_FORMAT_LEFT, 75);
  list_->AppendColumn("Time", wxLIST_FORMAT_LEFT, 120);
  list_->AppendColumn("Description", wxLIST_FORMAT_LEFT, 200);

  auto* filter_name_label = new wxStaticBox(this, wxID_ANY, "Name Filter:");
  filter_name_ctrl_ = new wxTextCtrl(this, wxID_ANY, "*");
  auto* filter_group_label = new wxStaticBox(this, wxID_ANY, "Group Filter:");
  filter_group_ctrl_ = new wxTextCtrl(this, wxID_ANY, "*");

  auto* filter_sizer = new wxBoxSizer(wxHORIZONTAL);
  filter_sizer->Add(filter_name_label, 0, wxLEFT, 5 );
  filter_sizer->Add(filter_name_ctrl_, 0, wxLEFT, 5 );
  filter_sizer->Add(filter_group_label, 0, wxLEFT, 10 );
  filter_sizer->Add(filter_group_ctrl_, 0, wxLEFT, 5 );

  auto* sizer = new wxBoxSizer(wxVERTICAL);
  sizer->Add(header_ctrl_, 0,  wxLEFT | wxRIGHT | wxALIGN_LEFT, 50);
  sizer->Add(list_, 1, wxALL | wxEXPAND, 0);
  sizer->Add(filter_sizer, 0, wxTOP | wxBOTTOM, 5);
  SetSizer(sizer);

}

ProjectDocument *MetricView::GetDocument() const {
  return view_ != nullptr ? view_->GetDoc() : nullptr;
}

IDatabase* MetricView::GetDatabase() const {
  if (const auto* doc = GetDocument(); doc != nullptr) {
    return doc->GetCurrentDatabase();
  }
  return nullptr;
}

void MetricView::Update() {
  if (GetDocument() == nullptr) {
    return;
  }
  Redraw();
}

void MetricView::Redraw() {
  if (!IsShown()) {
    return;
  }

  wxString header_text = MakeHeaderText();
  header_ctrl_->SetLabel(header_text);

  list_->DeleteAllItems();
  IDatabase* database = GetDatabase();
  if (database == nullptr) {
    return;
  }

  std::map<MetricSorted, const DbMetric*> sorted_list;
  for (const auto& metric: database->Metrics()) {
    if (!metric) {
      continue;
    }
    MetricSorted temp = {metric->Name(), metric->GroupName()};
    sorted_list.emplace(temp, metric.get());
  }


  long line = 0;
  for (const auto& [sorted, metric]: sorted_list) {
    if (metric == nullptr) {
      continue;
    }

    const auto index = list_->InsertItem(line,
                              wxString::FromUTF8(metric->Name()), -1);
    list_->SetItem(index, 1, wxString::FromUTF8(metric->GroupName()));
    // Todo: Fix the rest
    ++line;
  }

}

void MetricView::OnRightClick(wxListEvent& event) {
  const auto* doc = GetDocument();
  if (doc == nullptr) {
    return;
  }
  const auto* database = doc->GetCurrentDatabase();
  if (database == nullptr) {
    return;
  }
  wxMenu menu("Database");
  menu.Append(kIdEditDatabase, wxGetStockLabel(wxID_EDIT),
              "Change the database configuration");
  menu.Append(kIdDeleteDatabase, wxGetStockLabel(wxID_DELETE),
              "Delete the database configuration");
  menu.AppendSeparator();
  menu.Append(kIdActivateDatabase, "Activate", "Activate the database.");
  menu.Append(kIdDeactivateDatabase, "Deactivate", "Deactivate the database.");

  PopupMenu(&menu, event.GetPoint());
}

void MetricView::OnItemSelected(wxListEvent& event) {
  const wxListItem& current_item = event.GetItem();
  wxString name = list_->GetItemText(current_item, 0);
  // ToDo: Figure out what to do on metric selected
  /*
  if (auto* doc = GetDocument();doc != nullptr) {
    doc->SetCurrentItem(ProjectItemType::Databases, name.ToStdString());
  }
  */
}
wxString MetricView::MakeHeaderText() {
  auto* database = GetDatabase();
  if (database == nullptr) {
    return wxEmptyString;
  }
  std::ostringstream header_text;
  header_text << database->Name();
  if (!database->Filename().empty()) {
    try {
      path fullname(database->Filename());
      header_text << " - " << fullname.filename().string();
    } catch (const std::exception&) {
    }
  }
  if (!database->Description().empty()) {
    header_text << " - " << database->Description();
  }
  return wxString::FromUTF8(header_text.str());
}

}  // namespace bus
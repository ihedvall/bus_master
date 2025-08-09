/*
* Copyright 2025 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */
#include "environmentview.h"

#include <sstream>

#include "bus/project.h"
#include "projectdocument.h"
#include "projectview.h"
#include "windowid.h"

namespace {
constexpr int kEnvEnableBmp = 0;
constexpr int kEnvDisableBmp = 1;
constexpr int kEnvRunningBmp = 2;
constexpr int kEnvFailingBmp = 3;
constexpr int kEnvStoppedBmp = 4;
}

namespace bus {
wxBEGIN_EVENT_TABLE(EnvironmentView, wxPanel)
    EVT_LIST_ITEM_SELECTED(kIdEnvironmentList, EnvironmentView::OnItemSelected)
    EVT_LIST_ITEM_RIGHT_CLICK(kIdEnvironmentList, EnvironmentView::OnRightClick)
wxEND_EVENT_TABLE()

EnvironmentView::EnvironmentView(wxSplitterWindow* parent)
    : wxPanel(parent, kIdEnvironmentPanel),
      image_list_(16,16,false,5) {

  wxBitmap list_view("LIST_VIEW", wxBITMAP_TYPE_BMP_RESOURCE);

  auto* sizer = new wxBoxSizer(wxVERTICAL);
  list_ = new wxListView(this, kIdEnvironmentList,
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

ProjectDocument *EnvironmentView::GetDocument() const {
  return view_ != nullptr ? view_->GetDoc() : nullptr;
}

Project *EnvironmentView::GetProject() const {
  if (const auto* doc = GetDocument(); doc != nullptr) {
    return doc->GetProject();
  }
  return nullptr;
}

void EnvironmentView::Update() {
  if (GetDocument() == nullptr) {
    return;
  }
  Redraw();
}

void EnvironmentView::Redraw() {
  if (!IsShown()) {
    return;
  }

  list_->DeleteAllItems();
  Project* project = GetProject();
  if (project == nullptr) {
    return;
  }

  long line = 0;
  for (const auto& env : project->Environments()) {
    if (!env) {
      continue;
    }
    // ToDo: Fix Bitmap
    std::string status;
    int status_bmp = kEnvStoppedBmp;
    const std::string_view type = IEnvironment::TypeToString(env->Type());
    if (env->IsStarted() && env->IsOperable()) {
      status = "Running";
      status_bmp = kEnvRunningBmp;
    } else if (env->IsStarted() && !env->IsOperable()) {
      status = "Failing";
      status_bmp = kEnvFailingBmp;
    } else if (!env->IsStarted() && env->IsOperable()) {
      status = "Stopping";
    } else {
      status = "Stopped";
    }
    const auto index = list_->InsertItem(line, "",
                           env->IsEnabled() ? kEnvEnableBmp : kEnvDisableBmp);

    list_->SetItem(index, 1, wxString::FromUTF8(env->Name()));
    list_->SetItem(index, 2, wxString::FromUTF8(type));
    list_->SetItem(index, 3, wxString::FromUTF8(status), status_bmp);
    list_->SetItem(index, 4, wxString::FromUTF8(env->Description()));
    ++line;
  }

}

void EnvironmentView::OnRightClick(wxListEvent& event) {
  const auto* doc = GetDocument();
  if (doc == nullptr) {
    return;
  }
  const auto* env = doc->GetCurrentEnvironment();
  if (env == nullptr) {
    return;
  }
  wxMenu menu("Environment");
  menu.Append(kIdEditEnvironment, wxGetStockLabel(wxID_EDIT),
              "Change the environment configuration");
  menu.Append(kIdDeleteEnvironment, wxGetStockLabel(wxID_DELETE),
              "Delete the environment configuration");
  menu.AppendSeparator();
  menu.Append(kIdEnableEnvironment, "Enable", "Enable the environment.");
  menu.Append(kIdDisableEnvironment, "Disable", "Disable the environment.");
  menu.AppendSeparator();
  menu.Append(kIdStartEnvironment, "Start", "Start the environment");
  menu.Append(kIdStopEnvironment, "Stop", "Stop the environment");
  PopupMenu(&menu, event.GetPoint());
}

void EnvironmentView::OnItemSelected(wxListEvent& event) {
  const wxListItem& current_item = event.GetItem();
  wxString name = list_->GetItemText(current_item, 1);
  if (auto* doc = GetDocument();doc != nullptr) {
    doc->SetCurrentItem(ProjectItemType::Environments, name.ToStdString());
  }

}

}  // namespace bus
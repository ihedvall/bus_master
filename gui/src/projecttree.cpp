/*
 * Copyright 2025 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */
#include "projecttree.h"

#include <wx/splitter.h>

#include <set>
#include <string>

#include "projectdocument.h"
#include "projectframe.h"
#include "projectitemdata.h"
#include "projectview.h"
#include "windowid.h"

namespace {
#include "img/sub.xpm"
#include "img/tree_list.xpm"

// Bitmap index for the tree control (tree_list.bmp)
constexpr int TREE_ROOT = 0;
constexpr int TREE_ID = 1;
constexpr int TREE_HD = 2;
constexpr int TREE_FH_ROOT = 3;
constexpr int TREE_FH = 4;
constexpr int TREE_DG_ROOT = 5;
constexpr int TREE_DG = 6;
constexpr int TREE_AT_ROOT = 7;
constexpr int TREE_AT = 8;
constexpr int TREE_CH_ROOT = 9;
constexpr int TREE_CH = 10;
constexpr int TREE_EV_ROOT = 11;
constexpr int TREE_EV = 12;
constexpr int TREE_CG = 13;
constexpr int TREE_SI = 14;
constexpr int TREE_CN = 15;
constexpr int TREE_CC = 16;
constexpr int TREE_CA = 17;
constexpr int TREE_DT = 18;
constexpr int TREE_SR = 19;
constexpr int TREE_RD = 20;
constexpr int TREE_SD = 21;
constexpr int TREE_DL = 22;
constexpr int TREE_DZ = 23;
constexpr int TREE_HL = 24;
}

namespace bus {

wxBEGIN_EVENT_TABLE(ProjectTree, wxPanel)
        EVT_TREE_SEL_CHANGED(kIdLeftTree,ProjectTree::OnTreeSelected)
        EVT_TREE_ITEM_RIGHT_CLICK(kIdLeftTree, ProjectTree::OnTreeRightClick)
wxEND_EVENT_TABLE()

ProjectTree::ProjectTree(wxSplitterWindow *parent)
 : wxPanel(parent, kIdLeftPanel),
   image_list_(16,16,false,25)
{
#ifdef _WIN32
  wxBitmap tree_list("TREE_LIST", wxBITMAP_TYPE_BMP_RESOURCE);
#else
  wxBitmap tree_list {wxICON(tree_list)};
#endif

  tree_ = new wxTreeCtrl(this,kIdLeftTree);
  image_list_.Add(tree_list);
  tree_->SetImageList(&image_list_);

  auto* start_button = new wxButton(this, kIdStartSimulation,
    L"Start Simulation");
  auto* stop_button = new wxButton(this, kIdStopSimulation,
                                    L"Stop Simulation");
  auto* sizer = new wxBoxSizer(wxVERTICAL);
  sizer->SetMinSize(300, -1);
  sizer->Add(tree_, 1, wxALL | wxEXPAND, 0);
  sizer->Add(start_button, 0, wxALL | wxEXPAND, 5);
  sizer->Add(stop_button, 0, wxALL | wxEXPAND, 5);
  SetSizer(sizer);
}

ProjectDocument *ProjectTree::GetDocument() {
  return view_ != nullptr ? view_->GetDoc() : nullptr;
}

Project *ProjectTree::GetProject() {
  if (auto* doc = GetDocument(); doc != nullptr) {
    return doc->GetProject();
  }
  return nullptr;
}

void ProjectTree::RedrawRight() {
  auto* splitter = GetParent();
  if (splitter == nullptr) {
    return;
  }
  if (auto* frame = wxDynamicCast(splitter->GetParent(), ProjectFrame);
      frame != nullptr) {
    frame->RedrawRight();
  }
}

void ProjectTree::Update() {
  if (GetDocument() == nullptr) {
    return;
  }
  Redraw();
}

wxTreeItemId ProjectTree::FindCurrent(wxTreeItemId root_item) {
  const auto* doc = GetDocument();
  if (!tree_ || doc == nullptr) {
    return {};
  }
  for (wxTreeItemId item = root_item; item.IsOk();
            item = tree_->GetNextSibling(item)) {
    const auto* data =
      dynamic_cast<const ProjectItemData*>(tree_->GetItemData(item));
    if (data != nullptr
        && data->Type() == doc->GetCurrentType()
        && data->Id() == doc->GetCurrentId()) {
        return item;
    }
    if (tree_->ItemHasChildren(item)) {
      wxTreeItemIdValue cookie;
      wxTreeItemId find =
        FindCurrent( tree_->GetFirstChild(item, cookie));
      if (find.IsOk()) {
        return find;
      }
    }
  }
  return {};
}


void ProjectTree::Redraw() {
  tree_->DeleteAllItems();
  const auto* project = GetProject();
  if (project == nullptr) {
    return;
  }
  wxTreeItemId project_root = tree_->AddRoot(project->Name(),
    TREE_ROOT, TREE_ROOT,
    new ProjectItemData(ProjectItemType::Project, project->Name() ) );
  RedrawEnvironments(project_root);
  RedrawDatabases(project_root);
  RedrawSources(project_root);
  RedrawDestinations(project_root);

  if (wxTreeItemId current_item = FindCurrent(project_root);
      current_item.IsOk()) {
    tree_->SelectItem(current_item);
    tree_->EnsureVisible(current_item);
    tree_->Expand(current_item);
  } else {
    tree_->Expand(project_root);
  }
}

void ProjectTree::OnTreeSelected(wxTreeEvent& event) {
  auto* doc = GetDocument();
  if (doc == nullptr) {
    return;
  }

  const wxTreeItemId selected_item = event.GetItem();
  const wxTreeItemData* tree_data = tree_->GetItemData(selected_item);
  if (tree_data == nullptr) {
    doc->SetCurrentItem(ProjectItemType::Unknown,"");
  } else if (const auto* project_data =
                  dynamic_cast<const ProjectItemData*>(tree_data);
             project_data != nullptr) {
    doc->SetCurrentItem(project_data->Type(), project_data->Id());
  }
  RedrawRight();
}

void ProjectTree::OnTreeRightClick(wxTreeEvent& event) {
  OnTreeSelected(event);
  auto* doc = GetDocument();
  if (doc == nullptr) {
    return;
  }
  switch( doc->GetCurrentType()) {

    case ProjectItemType::Environment: {
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
      PopupMenu(&menu);
      break;
    }

    case ProjectItemType::Database: {
      wxMenu menu("Database");
      menu.Append(kIdEditDatabase, wxGetStockLabel(wxID_EDIT),
                  "Change the database configuration");
      menu.Append(kIdDeleteDatabase, wxGetStockLabel(wxID_DELETE),
                  "Delete the database configuration");
      menu.AppendSeparator();
      menu.Append(kIdActivateDatabase, "Enable", "Use this database");
      menu.Append(kIdDeactivateDatabase, "Disable",
                  "Don't use this database");
      PopupMenu(&menu, event.GetPoint());
      break;
    }

    case ProjectItemType::Source: {
      wxMenu menu("Source Task");
      menu.Append(kIdEditSource, wxGetStockLabel(wxID_EDIT),
                  "Change the selected task.");
      menu.Append(kIdDeleteSource, wxGetStockLabel(wxID_DELETE),
                  "Delete the selected task.");
      menu.AppendSeparator();
      menu.Append(kIdEnableSource, "Enable",
                  "Use this task in the simulation");
      menu.Append(kIdDisableSource, "Disable",
                  "Don't use this task in the simulation");
      PopupMenu(&menu, event.GetPoint());
      break;
    }

    default:
      break;
  }
}

void ProjectTree::RedrawEnvironments(wxTreeItemId& root_item) {
  wxTreeItemId env_root = tree_->AppendItem(root_item,
                     "Environments", TREE_DG_ROOT, TREE_DG_ROOT,
                     new ProjectItemData(ProjectItemType::Environments, "" ));
  const auto* project = GetProject();
  if (project == nullptr) {
    return;
  }
  std::set<std::string> sorted_list;
  for (const auto& env : project->Environments()) {
    if (!env) {
      continue;
    }
    sorted_list.emplace(env->Name());
  }
  for (const auto& name: sorted_list) {
    tree_->AppendItem(env_root, name, TREE_DG, TREE_DG,
         new ProjectItemData(ProjectItemType::Environment, name ));
  }
}

void ProjectTree::RedrawDatabases(wxTreeItemId& root_item) {
  wxTreeItemId db_root = tree_->AppendItem(root_item, "Databases",
                                           TREE_AT_ROOT, TREE_AT_ROOT,
                   new ProjectItemData(ProjectItemType::Databases, "" ));
  const auto* project = GetProject();
  if (project == nullptr) {
    return;
  }
  std::set<std::string> sorted_list;
  for (const auto& db : project->Databases()) {
    if (!db) {
      continue;
    }
    sorted_list.emplace(db->Name());
  }
  for (const auto& name: sorted_list) {
    tree_->AppendItem(db_root, name, TREE_AT, TREE_AT,
                      new ProjectItemData(ProjectItemType::Database, name ));
  }
}

void ProjectTree::RedrawSources(wxTreeItemId& root_item) {
  wxTreeItemId source_root = tree_->AppendItem(root_item, "Source Tasks",
                                           TREE_EV_ROOT, TREE_EV_ROOT,
                          new ProjectItemData(ProjectItemType::Sources, "" ));
  const auto* project = GetProject();
  if (project == nullptr) {
    return;
  }
  std::set<std::string> sorted_list;
  for (const auto& source : project->Sources()) {
    if (!source) {
      continue;
    }
    sorted_list.emplace(source->Name());
  }

  for (const auto& name: sorted_list) {
    tree_->AppendItem(source_root, name, TREE_EV, TREE_EV,
                      new ProjectItemData(ProjectItemType::Source, name ));
  }
}

void ProjectTree::RedrawDestinations(wxTreeItemId& root_item) {
  wxTreeItemId dest_root = tree_->AppendItem(root_item, "Destination Tasks",
                                               TREE_CH_ROOT, TREE_CH_ROOT,
                     new ProjectItemData(ProjectItemType::Destinations, "" ));
  const auto* project = GetProject();
  if (project == nullptr) {
    return;
  }
  std::set<std::string> sorted_list;
  for (const auto& dest : project->Destinations()) {
    if (!dest) {
      continue;
    }
    sorted_list.emplace(dest->Name());
  }
  for (const auto& name: sorted_list) {
    tree_->AppendItem(dest_root, name, TREE_EV, TREE_EV,
                      new ProjectItemData(ProjectItemType::Destination, name ));
  }
}

} // bus
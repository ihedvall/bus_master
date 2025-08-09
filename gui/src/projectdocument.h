/*
 * Copyright 2025 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */
#pragma once
#include <wx/docview.h>
#include <wx/wx.h>

#include <memory>

#include "bus/project.h"
#include "projectitemdata.h"

namespace bus {

class MainFrame;

class ProjectDocument : public wxDocument {
 public:
  ProjectDocument() = default;

  void SetCurrentItem(ProjectItemType type, const std::string& id) {
    current_type_ = type;
    current_id_ = id;
  }

  [[nodiscard]] ProjectItemType GetCurrentType() const { return current_type_; }
  [[nodiscard]] const std::string& GetCurrentId() const { return current_id_; }

  [[nodiscard]] Project* GetProject() const;
  [[nodiscard]] IEnvironment* GetCurrentEnvironment() const;
  [[nodiscard]] IDatabase* GetCurrentDatabase() const;
  [[nodiscard]] ISource* GetCurrentSource() const;
  [[nodiscard]] IDestination* GetCurrentDestination() const;
  [[nodiscard]] MainFrame* GetMainFrame() const;

 protected:
  bool OnNewDocument() override;
  bool DoOpenDocument(const wxString& filename) override;
  bool DoSaveDocument(const wxString& filename) override;
  bool OnCloseDocument() override;

 private:
  std::unique_ptr<Project> project_;
  ProjectItemType current_type_ = ProjectItemType::Project;
  std::string current_id_;

  void VerifyProjectPath();

  void OnUpdateProjectExist(wxUpdateUIEvent& event);

  void OnUpdateEnvironmentSelected(wxUpdateUIEvent& event);
  void OnUpdateEnableEnvironment(wxUpdateUIEvent& event);
  void OnUpdateDisableEnvironment(wxUpdateUIEvent& event);
  void OnUpdateStartEnvironment(wxUpdateUIEvent& event);
  void OnUpdateStopEnvironment(wxUpdateUIEvent& event);

  void OnUpdateDatabaseSelected(wxUpdateUIEvent& event);
  void OnUpdateActivateDatabase(wxUpdateUIEvent& event);
  void OnUpdateDeactivateDatabase(wxUpdateUIEvent& event);

  void OnUpdateSourceSelected(wxUpdateUIEvent& event);
  void OnUpdateEnableSource(wxUpdateUIEvent& event);
  void OnUpdateDisableSource(wxUpdateUIEvent& event);

  void OnEditProject(wxCommandEvent& event);

  void OnAddBrokerEnvironment(wxCommandEvent& event);
  void OnEditEnvironment(wxCommandEvent& event);
  void OnDeleteEnvironment(wxCommandEvent& event);
  void OnEnableEnvironment(wxCommandEvent& event);
  void OnDisableEnvironment(wxCommandEvent& event);
  void OnStartEnvironment(wxCommandEvent& event);
  void OnStopEnvironment(wxCommandEvent& event);

  void OnAddDbcDatabase(wxCommandEvent& event);
  void OnEditDatabase(wxCommandEvent& event);
  void OnDeleteDatabase(wxCommandEvent& event);
  void OnActivateDatabase(wxCommandEvent& event);
  void OnDeactivateDatabase(wxCommandEvent& event);

  void OnAddUnknownSource(wxCommandEvent& event);
  void OnAddMdfSource(wxCommandEvent& event);
  void OnEditSource(wxCommandEvent& event);
  void OnDeleteSource(wxCommandEvent& event);
  void OnEnableSource(wxCommandEvent& event);
  void OnDisableSource(wxCommandEvent& event);

  void OnAddUnknownDestination(wxCommandEvent& event);
  wxDECLARE_DYNAMIC_CLASS(ProjectDocument);
  wxDECLARE_EVENT_TABLE();
};



}




/*
 * Copyright 2025 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */
#include "projectdocument.h"

#include <util/logstream.h>
#include <util/stringutil.h>
#include <util/systeminfo.h>
#include <wx/msgdlg.h>

#include <filesystem>
#include <sstream>

#include "bus/dbcdatabase.h"

#include "busmaster.h"
#include "environmentdialog.h"
#include "environmenttypedialog.h"
#include "databasedialog.h"
#include "unknownsourcedialog.h"
#include "mdfdialog.h"
#include "unknowndestinationdialog.h"
#include "mainframe.h"
#include "projectdialog.h"
#include "windowid.h"

using namespace std::filesystem;

using namespace util::log;
using namespace util::supervise;
using namespace util::string;

namespace bus {
wxIMPLEMENT_DYNAMIC_CLASS(ProjectDocument, wxDocument)

wxBEGIN_EVENT_TABLE(ProjectDocument, wxDocument)
  EVT_UPDATE_UI(kIdEditProject, ProjectDocument::OnUpdateProjectExist)
  EVT_MENU(kIdEditProject, ProjectDocument::OnEditProject)

  EVT_UPDATE_UI(kIdAddEnvironment, ProjectDocument::OnUpdateProjectExist)
  EVT_UPDATE_UI(kIdAddBrokerEnvironment, ProjectDocument::OnUpdateProjectExist)
  EVT_MENU(kIdAddBrokerEnvironment, ProjectDocument::OnAddBrokerEnvironment)

  EVT_UPDATE_UI(kIdEditEnvironment, ProjectDocument::OnUpdateEnvironmentSelected)
  EVT_MENU(kIdEditEnvironment, ProjectDocument::OnEditEnvironment)

  EVT_UPDATE_UI(kIdDeleteEnvironment, ProjectDocument::OnUpdateEnvironmentSelected)
  EVT_MENU(kIdDeleteEnvironment, ProjectDocument::OnDeleteEnvironment)

  EVT_UPDATE_UI(kIdEnableEnvironment, ProjectDocument::OnUpdateEnableEnvironment)
  EVT_MENU(kIdEnableEnvironment, ProjectDocument::OnEnableEnvironment)

  EVT_UPDATE_UI(kIdDisableEnvironment, ProjectDocument::OnUpdateDisableEnvironment)
  EVT_MENU(kIdDisableEnvironment, ProjectDocument::OnDisableEnvironment)

  EVT_UPDATE_UI(kIdStartEnvironment, ProjectDocument::OnUpdateStartEnvironment)
  EVT_MENU(kIdStartEnvironment, ProjectDocument::OnStartEnvironment)

  EVT_UPDATE_UI(kIdStopEnvironment, ProjectDocument::OnUpdateStopEnvironment)
  EVT_MENU(kIdStopEnvironment, ProjectDocument::OnStopEnvironment)

  EVT_UPDATE_UI(kIdAddDatabase, ProjectDocument::OnUpdateProjectExist)
  EVT_UPDATE_UI(kIdAddDbcDatabase, ProjectDocument::OnUpdateProjectExist)
  EVT_MENU(kIdAddDbcDatabase, ProjectDocument::OnAddDbcDatabase)

  EVT_UPDATE_UI(kIdEditDatabase, ProjectDocument::OnUpdateDatabaseSelected)
  EVT_MENU(kIdEditDatabase, ProjectDocument::OnEditDatabase)

  EVT_UPDATE_UI(kIdDeleteDatabase, ProjectDocument::OnUpdateDatabaseSelected)
  EVT_MENU(kIdDeleteDatabase, ProjectDocument::OnDeleteDatabase)

  EVT_UPDATE_UI(kIdActivateDatabase, ProjectDocument::OnUpdateActivateDatabase)
  EVT_MENU(kIdActivateDatabase, ProjectDocument::OnActivateDatabase)

  EVT_UPDATE_UI(kIdDeactivateDatabase, ProjectDocument::OnUpdateDeactivateDatabase)
  EVT_MENU(kIdDeactivateDatabase, ProjectDocument::OnDeactivateDatabase)

  EVT_UPDATE_UI(kIdAddSource, ProjectDocument::OnUpdateProjectExist)
  EVT_UPDATE_UI(kIdAddUnknownSource, ProjectDocument::OnUpdateProjectExist)
  EVT_UPDATE_UI(kIdAddMdfSource, ProjectDocument::OnUpdateProjectExist)
  EVT_UPDATE_UI(kIdEditSource, ProjectDocument::OnUpdateSourceSelected)
  EVT_UPDATE_UI(kIdDeleteSource, ProjectDocument::OnUpdateSourceSelected)
  EVT_UPDATE_UI(kIdEnableSource, ProjectDocument::OnUpdateEnableSource)
  EVT_UPDATE_UI(kIdDisableSource, ProjectDocument::OnUpdateDisableSource)

  EVT_MENU(kIdAddUnknownSource, ProjectDocument::OnAddUnknownSource)
  EVT_MENU(kIdAddMdfSource, ProjectDocument::OnAddMdfSource)
  EVT_MENU(kIdEditSource, ProjectDocument::OnEditSource)
  EVT_MENU(kIdDeleteSource, ProjectDocument::OnDeleteSource)
  EVT_MENU(kIdEnableSource, ProjectDocument::OnEnableSource)
  EVT_MENU(kIdDisableSource, ProjectDocument::OnDisableSource)

  EVT_UPDATE_UI(kIdAddDestination, ProjectDocument::OnUpdateProjectExist)
  EVT_UPDATE_UI(kIdAddUnknownDestination, ProjectDocument::OnUpdateProjectExist)
  EVT_UPDATE_UI(kIdAddMdfDestination, ProjectDocument::OnUpdateProjectExist)
  EVT_MENU(kIdAddUnknownDestination, ProjectDocument::OnAddUnknownDestination)
wxEND_EVENT_TABLE()


Project* ProjectDocument::GetProject() const {
  return project_ ? project_.get() : nullptr;
}

bool ProjectDocument::OnNewDocument() {
  const bool new_doc = wxDocument::OnNewDocument( );
  if (!new_doc) {
    return false;
  }

  project_ = std::make_unique<Project>();
  auto& app = wxGetApp();
  ProjectDialog dialog(app.GetTopWindow());
  const int result = dialog.ShowModal();
  if (result == wxID_CANCEL) {
    return false;
  }

  // Update the project name
  dialog.GetProject(*project_);
  VerifyProjectPath();
  Modify(true);
  SetCurrentItem(ProjectItemType::Project, project_->Name());

  const wxString filename(project_->ConfigFile());
  SetFilename(filename, false);
  UpdateAllViews();
  return true;
}

void ProjectDocument::VerifyProjectPath() {
  if (!project_) {
    return;
  }
  const auto& app = wxGetApp();
  const std::string app_name = app.GetAppName().ToStdString();
  const SystemInfo sys_info(app_name);
  std::string config_file = project_->ConfigFile();
  try {

    if (config_file.empty()) {
      path config_path(sys_info.ConfigPath());
      create_directories(config_path);
      config_path.append("bus_master.xml");
      config_file = config_path.string();
      LOG_TRACE() << "Changed the configuration filename. File: "
                  << config_file;
    } else {
      const path fullname(config_file);
      if (!fullname.has_parent_path()) {
        path config_path(sys_info.ConfigPath());
        create_directories(config_path);
        config_path.append(config_file);
        config_file = config_path.string();
        LOG_TRACE() << "Changed the configuration path. File: "
                    << config_file;
      } else if (!exists(fullname)) {
        const path config_path(sys_info.ConfigPath());
        create_directories(config_path);
      }
    }
    if (!IEquals(config_file, project_->ConfigFile())) {
      Modify(true);
      project_->ConfigFile(config_file);
    }
  } catch (const std::exception& err) {
    LOG_ERROR() << "Error verifying the project path. Error: " << err.what();
  }

}

bool ProjectDocument::DoSaveDocument(const wxString& filename) {
  if (!project_ || !IsModified()) {
    return true;
  }
  const bool save = project_->WriteConfig();
  if (save) {
    Modify(false);
  }
  return save;
}

bool ProjectDocument::DoOpenDocument(const wxString& filename) {

  project_ = std::make_unique<Project>();
  project_->ConfigFile(filename.ToStdString());
  VerifyProjectPath();

  const bool read = project_->ReadConfig();
  if (!read) {
    LOG_ERROR() << "Error reading the project. File: " << project_->ConfigFile();
  } else {
    LOG_INFO() << "Opened the project. File: " << project_->ConfigFile();
  }
  return read;
}

bool ProjectDocument::OnCloseDocument() {
  if (project_ && IsModified() && !project_->ConfigFile().empty()) {
    const bool save = project_->WriteConfig();
    if (!save) {
      LOG_ERROR() << "Error saving project. File: " << project_->ConfigFile();
    } else {
      LOG_INFO() << "Saved the project. File: " << project_->ConfigFile();
    }
    project_.reset();
  }
  Modify(false);
  return wxDocument::OnCloseDocument();
}

void ProjectDocument::OnUpdateProjectExist(wxUpdateUIEvent& event) {
  if (const auto* project = GetProject(); project != nullptr) {
    event.Enable(true);
  } else {
    event.Enable(false);
  }
}

void ProjectDocument::OnUpdateEnvironmentSelected(wxUpdateUIEvent& event) {
  if (const IEnvironment* env = GetCurrentEnvironment(); env != nullptr) {
    event.Enable(true);
  } else {
    event.Enable(false);
  }
}

void ProjectDocument::OnUpdateEnableEnvironment(wxUpdateUIEvent& event) {
  if (const IEnvironment* env = GetCurrentEnvironment();
      env != nullptr) {
    event.Enable(!env->IsEnabled());
  } else {
    event.Enable(false);
  }
}

void ProjectDocument::OnUpdateDisableEnvironment(wxUpdateUIEvent& event) {
  if (const IEnvironment* env = GetCurrentEnvironment();
      env != nullptr) {
    event.Enable(env->IsEnabled());
  } else {
    event.Enable(false);
  }
}

void ProjectDocument::OnUpdateStartEnvironment(wxUpdateUIEvent& event) {
  if (const IEnvironment* env = GetCurrentEnvironment();
      env != nullptr) {
    event.Enable(!env->IsStarted());
  } else {
    event.Enable(false);
  }
}

void ProjectDocument::OnUpdateStopEnvironment(wxUpdateUIEvent& event) {
  if (const IEnvironment* env = GetCurrentEnvironment();
    env != nullptr) {
    event.Enable(env->IsStarted());
  } else {
    event.Enable(false);
  }
}

void ProjectDocument::OnUpdateDatabaseSelected(wxUpdateUIEvent& event) {
  if (const IDatabase* db = GetCurrentDatabase(); db != nullptr) {
    event.Enable(true);
  } else {
    event.Enable(false);
  }
}

void ProjectDocument::OnUpdateActivateDatabase(wxUpdateUIEvent& event) {
  if (const IDatabase* db = GetCurrentDatabase(); db != nullptr) {
    event.Enable(!db->IsEnabled());
  } else {
    event.Enable(false);
  }
}

void ProjectDocument::OnUpdateDeactivateDatabase(wxUpdateUIEvent& event) {
  if (const IDatabase* db = GetCurrentDatabase(); db != nullptr) {
    event.Enable(db->IsEnabled());
  } else {
    event.Enable(false);
  }
}

void ProjectDocument::OnEditProject(wxCommandEvent& event) {
  auto* main_frame = GetMainFrame();
  auto* project = GetProject();

  if (project != nullptr && main_frame != nullptr) {
    ProjectDialog dialog(main_frame);
    dialog.SetProject(*project);
    if (dialog.ShowModal() == wxID_SAVE) {
      const bool modified = dialog.GetProject(*project);
      if ( modified) {
        Modify(true);
        SetCurrentItem(ProjectItemType::Project,
                            project->Name());
        UpdateAllViews();
      }
    }
  }
}

void ProjectDocument::OnAddBrokerEnvironment(wxCommandEvent& event) {
  auto* frame = GetMainFrame();
  auto* project = GetProject();
  const auto* current_env = GetCurrentEnvironment();

  if (project == nullptr || frame == nullptr) {
    return;
  }

  std::vector<std::string> invalid_names;
  for (const auto& env : project->Environments()) {
    if (env) {
      invalid_names.push_back(env->Name());
    }
  }

  constexpr auto type = TypeOfEnvironment::BrokerEnvironment;
  IEnvironment new_env;
  if (current_env != nullptr) {
    new_env = *current_env;
    new_env.Name("");
  }
  EnvironmentDialog dialog(frame);
  dialog.SetEnvironment(new_env);
  dialog.SetInvalidNames(invalid_names);
  if (dialog.ShowModal() != wxID_SAVE) {
    return;
  }
  dialog.GetEnvironment(new_env);

  IEnvironment* env = project_->CreateEnvironment(type);
  if (env == nullptr ) {
    LOG_ERROR() << "Error creating environment. Type: "
                << IEnvironment::TypeToString(type);
    return;
  }

  *env = new_env;
  SetCurrentItem(ProjectItemType::Environment, env->Name());
  Modify(true);
  UpdateAllViews();

}

void ProjectDocument::OnEditEnvironment(wxCommandEvent& event) {
  auto* frame = GetMainFrame();
  auto* current_env = GetCurrentEnvironment();
  auto* project = GetProject();
  if (current_env == nullptr || frame == nullptr || project == nullptr) {
    return;
  }

  std::vector<std::string> invalid_names;
  for (const auto& env : project->Environments()) {
    if (env && env->Name() != current_env->Name()) {
      invalid_names.push_back(env->Name());
    }
  }
  EnvironmentDialog dialog(frame);
  dialog.SetEnvironment(*current_env);
  dialog.SetInvalidNames(invalid_names);
  if (dialog.ShowModal() != wxID_SAVE) {
    return;
  }
  const bool modified = dialog.GetEnvironment(*current_env);

  // If it changed its name
  SetCurrentItem(ProjectItemType::Environment, current_env->Name());
  if (modified ) {
    Modify(true);
    UpdateAllViews();
  }
}

void ProjectDocument::OnDeleteEnvironment(wxCommandEvent& event) {
  auto* frame = GetMainFrame();
  auto* current_env = GetCurrentEnvironment();
  auto* project = GetProject();
  if (current_env == nullptr || frame == nullptr || project == nullptr) {
    return;
  }

  std::ostringstream msg;
  msg << "Do you want to delete this environment?" << std::endl;
  msg << "Environment: " << current_env->Name();

  int result = wxMessageBox(msg.str(), "Delete Environment",
         wxYES_NO | wxICON_QUESTION | wxCENTRE, frame);
  if (result != wxYES) {
    return;
  }
  project_->DeleteEnvironment(current_env->Name());
  SetCurrentItem(ProjectItemType::Environments, "");
  Modify(true);
  UpdateAllViews();
}

void ProjectDocument::OnEnableEnvironment(wxCommandEvent& event) {
  if (auto* current_env = GetCurrentEnvironment(); current_env != nullptr) {
    current_env->Enable(true);
    Modify(true);
    UpdateAllViews();
  }
}

void ProjectDocument::OnDisableEnvironment(wxCommandEvent& event) {
  if (auto* current_env = GetCurrentEnvironment(); current_env != nullptr) {
    if (current_env->IsStarted()) {
      current_env->Stop();
    }
    current_env->Enable(false);
    Modify(true);
    UpdateAllViews();
  }
}

void ProjectDocument::OnStartEnvironment(wxCommandEvent& event) {
  auto* current_env = GetCurrentEnvironment();

  if (current_env == nullptr) {
    return;
  }
  current_env->Start();
  UpdateAllViews();
}

void ProjectDocument::OnStopEnvironment(wxCommandEvent& event) {
  auto* current_env = GetCurrentEnvironment();

  if (current_env == nullptr) {
    return;
  }
  current_env->Stop();
  UpdateAllViews();
}

void ProjectDocument::OnAddDbcDatabase(wxCommandEvent& event) {
  auto* frame = GetMainFrame();
  auto* project = GetProject();
  if (project == nullptr || frame == nullptr) {
    LOG_ERROR() << "No project or frame defined. Invalid use the function.";
    return;
  }


  std::vector<std::string> invalid_names;
  for (const auto& db : project->Databases()) {
    if (db) {
      invalid_names.push_back(db->Name());
    }
  }

  constexpr auto type = TypeOfDatabase::DbcFile;
  IDatabase new_db;
  DatabaseDialog dialog(frame, type);
  dialog.SetDatabase(new_db);
  dialog.SetInvalidNames(invalid_names);
  if (dialog.ShowModal() != wxID_SAVE) {
    return;
  }
  dialog.GetDatabase(new_db);

  IDatabase* db = project_->CreateDatabase(type);
  if (db == nullptr ) {
    LOG_ERROR() << "Database create error.";
    return;
  }
  *db = new_db;
  SetCurrentItem(ProjectItemType::Database, db->Name());
  Modify(true);
  UpdateAllViews();
}

void ProjectDocument::OnEditDatabase(wxCommandEvent& event) {
  auto* frame = GetMainFrame();
  auto* current_db = GetCurrentDatabase();
  auto* project = GetProject();
  if (current_db == nullptr || frame == nullptr || project == nullptr) {
    return;
  }

  std::vector<std::string> invalid_names;
  for (const auto& db : project->Databases()) {
    if (db && db->Name() != current_db->Name()) {
      invalid_names.push_back(db->Name());
    }
  }

  DatabaseDialog dialog(frame, current_db->Type());
  dialog.SetDatabase(*current_db);
  dialog.SetInvalidNames(invalid_names);
  if (dialog.ShowModal() != wxID_SAVE) {
    return;
  }
  const bool modified = dialog.GetDatabase(*current_db);

  // If it changed its name
  SetCurrentItem(ProjectItemType::Database, current_db->Name());
  if (modified ) {
    Modify(true);
    UpdateAllViews();
  }
}

void ProjectDocument::OnDeleteDatabase(wxCommandEvent& event) {
  auto* frame = GetMainFrame();
  auto* current_db = GetCurrentDatabase();
  auto* project = GetProject();
  if (current_db == nullptr || frame == nullptr || project == nullptr) {
    return;
  }

  std::ostringstream msg;
  msg << "Do you want to delete this database?" << std::endl;
  msg << "Database: " << current_db->Name();

  int result = wxMessageBox(msg.str(), "Delete Database",
                            wxYES_NO | wxICON_QUESTION | wxCENTRE, frame);
  if (result != wxYES) {
    return;
  }
  project_->DeleteDatabase(current_db->Name());
  SetCurrentItem(ProjectItemType::Databases, "");
  Modify(true);
  UpdateAllViews();
}

void ProjectDocument::OnActivateDatabase(wxCommandEvent& event) {
  auto* current_db = GetCurrentDatabase();

  if (current_db == nullptr) {
    return;
  }
  Modify(true);
  current_db->Enable(true);
  UpdateAllViews();
}

void ProjectDocument::OnDeactivateDatabase(wxCommandEvent& event) {
  auto* current_db = GetCurrentDatabase();

  if (current_db == nullptr) {
    return;
  }
  Modify(true);
  current_db->Enable(false);
  UpdateAllViews();
}

void ProjectDocument::OnUpdateSourceSelected(wxUpdateUIEvent& event) {
  if (const ISource* source = GetCurrentSource(); source != nullptr) {
    event.Enable(true);
  } else {
    event.Enable(false);
  }
}

void ProjectDocument::OnUpdateEnableSource(wxUpdateUIEvent& event) {
  if (const ISource* source = GetCurrentSource(); source != nullptr) {
    event.Enable(!source->IsEnabled());
  } else {
    event.Enable(false);
  }
}

void ProjectDocument::OnUpdateDisableSource(wxUpdateUIEvent& event) {
  if (const ISource* source = GetCurrentSource(); source != nullptr) {
    event.Enable(source->IsEnabled());
  } else {
    event.Enable(false);
  }
}

void ProjectDocument::OnAddUnknownSource(wxCommandEvent& event) {
  auto* frame = GetMainFrame();
  auto* project = GetProject();

  if (project == nullptr || frame == nullptr) {
    return;
  }

  constexpr TypeOfSource type = TypeOfSource::Unknown;
  ISource new_source;
  std::vector<std::string> invalid_names;
  for (const auto& source : project->Sources()) {
    if (source) {
      invalid_names.push_back(source->Name());
    }
  }
  UnknownSourceDialog dialog(frame);
  dialog.SetSource(new_source);
  dialog.SetInvalidNames(invalid_names);
  if (dialog.ShowModal() != wxID_SAVE) {
    return;
  }
  dialog.GetSource(new_source);

  ISource* source = project_->CreateSource(type);
  if (source == nullptr ) {
    return;
  }
  *source = new_source;
  SetCurrentItem(ProjectItemType::Source, source->Name());
  Modify(true);
  UpdateAllViews();
}

void ProjectDocument::OnAddMdfSource(wxCommandEvent& event) {
  auto* frame = GetMainFrame();
  auto* project = GetProject();

  if (project == nullptr || frame == nullptr) {
    return;
  }

  constexpr TypeOfSource type = TypeOfSource::Mdf;
  ISource new_source;
  std::vector<std::string> invalid_names;
  for (const auto& source : project->Sources()) {
    if (source) {
      invalid_names.push_back(source->Name());
    }
  }
  MdfDialog dialog(frame);
  dialog.SetSource(new_source);
  dialog.SetInvalidNames(invalid_names);
  if (dialog.ShowModal() != wxID_SAVE) {
    return;
  }
  dialog.GetSource(new_source);

  ISource* source = project_->CreateSource(type);
  if (source == nullptr ) {
    return;
  }
  *source = new_source;
  SetCurrentItem(ProjectItemType::Source, source->Name());
  Modify(true);
  UpdateAllViews();
}

void ProjectDocument::OnEditSource(wxCommandEvent& event) {
  auto* frame = GetMainFrame();
  auto* current_source = GetCurrentSource();
  auto* project = GetProject();
  if (current_source == nullptr || frame == nullptr || project == nullptr) {
    return;
  }

  std::vector<std::string> invalid_names;
  for (const auto& source : project->Sources()) {
    if (source && source->Name() != current_source->Name()) {
      invalid_names.push_back(source->Name());
    }
  }
  bool modified = false;
  switch (current_source->Type()) {
    case TypeOfSource::Mdf: {
      MdfDialog dialog(frame);
      dialog.SetSource(*current_source);
      dialog.SetInvalidNames(invalid_names);
      if (dialog.ShowModal() != wxID_SAVE) {
        return;
      }
      modified = dialog.GetSource(*current_source);
      break;
    }

    default: {
      UnknownSourceDialog dialog(frame);
      dialog.SetSource(*current_source);
      dialog.SetInvalidNames(invalid_names);
      if (dialog.ShowModal() != wxID_SAVE) {
        return;
      }
      modified = dialog.GetSource(*current_source);
      break;
    }
  }

  SetCurrentItem(ProjectItemType::Source, current_source->Name());
  if (modified ) {
    Modify(true);
    UpdateAllViews();
  }
}

void ProjectDocument::OnDeleteSource(wxCommandEvent& event) {
  auto* frame = GetMainFrame();
  auto* current_source = GetCurrentSource();
  auto* project = GetProject();
  if (current_source == nullptr || frame == nullptr || project == nullptr) {
    return;
  }

  std::ostringstream msg;
  msg << "Do you want to delete this source task?" << std::endl;
  msg << "Source Task: " << current_source->Name();

  const int result = wxMessageBox(msg.str(), "Delete Source Task",
                            wxYES_NO | wxICON_QUESTION | wxCENTRE, frame);
  if (result != wxYES) {
    return;
  }
  project_->DeleteSource(current_source->Name());
  SetCurrentItem(ProjectItemType::Sources, "");
  Modify(true);
  UpdateAllViews();
}
void ProjectDocument::OnEnableSource(wxCommandEvent& event) {
  auto* current_source = GetCurrentSource();
  if (current_source == nullptr) {
    return;
  }
  Modify(true);
  current_source->Enable(true);
  UpdateAllViews();
}

void ProjectDocument::OnDisableSource(wxCommandEvent& event) {
  auto* current_source = GetCurrentSource();
  if (current_source == nullptr) {
    return;
  }
  Modify(true);
  current_source->Enable(false);
  UpdateAllViews();
}

void ProjectDocument::OnAddUnknownDestination(wxCommandEvent& event) {
  auto* frame = GetMainFrame();
  auto* project = GetProject();

  if (project == nullptr || frame == nullptr) {
    return;
  }

  constexpr TypeOfDestination type = TypeOfDestination::Unknown;
  IDestination new_dest;
  std::vector<std::string> invalid_names;
  for (const auto& dest : project->Destinations()) {
    if (dest) {
      invalid_names.push_back(dest->Name());
    }
  }
  UnknownDestinationDialog dialog(frame);
  dialog.SetDestination(new_dest);
  dialog.SetInvalidNames(invalid_names);
  if (dialog.ShowModal() != wxID_SAVE) {
    return;
  }
  dialog.GetDestination(new_dest);

  IDestination* dest = project_->CreateDestination(type);
  if (dest == nullptr ) {
    return;
  }
  *dest = new_dest;
  SetCurrentItem(ProjectItemType::Destination, dest->Name());
  Modify(true);
  UpdateAllViews();
}

MainFrame* ProjectDocument::GetMainFrame() const {
  const auto& app = wxGetApp();
  return dynamic_cast<MainFrame*>(app.GetTopWindow());
}

IEnvironment* ProjectDocument::GetCurrentEnvironment() const {
  if (project_) {
    switch (current_type_) {
      case ProjectItemType::Environment:
      case ProjectItemType::Environments:
        return project_->GetEnvironment(GetCurrentId());

      default:
        break;
    }
  }
  return nullptr;
}

IDatabase* ProjectDocument::GetCurrentDatabase() const {
  if (project_) {
    switch (current_type_) {
      case ProjectItemType::Databases:
      case ProjectItemType::Database:
        return project_->GetDatabase(GetCurrentId());

      default:
        break;
    }
  }
  return nullptr;
}

ISource* ProjectDocument::GetCurrentSource() const {
  if (project_) {
    switch (current_type_) {
      case ProjectItemType::Source:
      case ProjectItemType::Sources:
        return project_->GetSource(GetCurrentId());

      default:
        break;
    }
  }
  return nullptr;
}

IDestination* ProjectDocument::GetCurrentDestination() const {
  if (!project_ || current_type_ != ProjectItemType::Destination) {
    return nullptr;
  }
  return project_->GetDestination(GetCurrentId());
}



} // namespace bus
/*
 * Copyright 2025 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */

#include "mainframe.h"

#include <util/logstream.h>
#include <util/timestamp.h>
#include <util/utilfactory.h>
#include <wx/aboutdlg.h>
#include <wx/config.h>

#include <array>
#include <string>

#include "projectdocument.h"
#include "projectframe.h"
#include "windowid.h"

using namespace util;
using namespace util::log;
using namespace util::time;


namespace bus {

wxBEGIN_EVENT_TABLE(MainFrame, wxDocParentFrame)
  EVT_MENU(wxID_ABOUT, MainFrame::OnAbout)
  EVT_CLOSE(MainFrame::OnClose)
  EVT_DROP_FILES(MainFrame::OnDropFiles)
  EVT_TIMER(kIdTimer, MainFrame::OnTimer)

  EVT_UPDATE_UI(kIdEditProject, MainFrame::OnUpdateNoDocument)
  EVT_UPDATE_UI_RANGE(kIdAddEnvironment,kIdDeleteDestination,
                                        MainFrame::OnUpdateNoDocument)
wxEND_EVENT_TABLE()

MainFrame::MainFrame(wxDocManager* doc_manager,
                                     const wxString& title,
                                     const wxPoint& start_pos,
                                     const wxSize& start_size, bool maximized)
    : wxDocParentFrame(doc_manager, nullptr, wxID_ANY, title, start_pos,
                       start_size) {
#ifdef _WIN32
  wxIcon app("APP_ICON", wxBITMAP_TYPE_ICO_RESOURCE);
#else
  wxIcon app{wxICON(app)};
#endif
  SetIcon(app);
  wxWindow::SetName("BusMasterTop");
#if (_MSC_VER)
  wxTopLevelWindowMSW::Maximize(maximized);
#else
  wxTopLevelWindowNative::Maximize(maximized);
#endif
  wxWindow::DragAcceptFiles(true);

  auto* app_config = wxConfig::Get();

  // PROJECT
  auto* menu_project = new wxMenu;
  menu_project->Append(wxID_NEW);
  menu_project->Append(wxID_OPEN);
  menu_project->AppendSeparator();
  menu_project->Append(kIdEditProject, wxGetStockLabel(wxID_EDIT),
                       "Modifies the project");
  menu_project->Append(wxID_SAVE);
  menu_project->Append(wxID_SAVEAS);
  menu_project->Append(wxID_EXIT);

  doc_manager->FileHistoryUseMenu(menu_project);
  doc_manager->FileHistoryLoad(*app_config);

  // ENVIRONMENTS
  auto* menu_env = new wxMenu;
  menu_env->Append(kIdAddEnvironment, wxGetStockLabel(wxID_ADD),
                   "Add an environment");
  menu_env->Append(kIdEditEnvironment, wxGetStockLabel(wxID_EDIT),
                   "Change an environment");
  menu_env->Append(kIdDeleteEnvironment, wxGetStockLabel(wxID_DELETE),
                   "Delete an environment");
  menu_env->AppendSeparator();
  menu_env->Append(kIdStartEnvironment, "Start","Starts the environment");
  menu_env->Append(kIdStopEnvironment, "Stop","Stops the environment");

  // DATABASES
  auto* menu_db = new wxMenu;
  menu_db->Append(kIdAddDatabase, wxGetStockLabel(wxID_ADD),
                   "Add a database");
  menu_db->Append(kIdEditDatabase, wxGetStockLabel(wxID_EDIT),
                   "Change a database");
  menu_db->Append(kIdDeleteDatabase, wxGetStockLabel(wxID_DELETE),
                   "Delete a database");
  menu_db->AppendSeparator();
  menu_db->Append(kIdActivateDatabase, "Activate","Activates the database. Start using the database.");
  menu_db->Append(kIdStopEnvironment, "Deactivate","Deactivate the database. Stop using the database.");

  // SOURCES
  auto* menu_add_source = new wxMenu;
  menu_add_source->Append(kIdAddUnknownSource, "Unknown",
                          "Add an unknown source.");
  menu_add_source->Append(kIdAddUnknownSource, "MDF Log File",
                          "Add an MDF log file source.");

  auto* menu_source = new wxMenu;
  menu_source->Append(kIdAddSource, wxGetStockLabel(wxID_ADD), menu_add_source,
                  "Add a data source");
  menu_source->Append(kIdEditSource, wxGetStockLabel(wxID_EDIT),
                  "Change a data source");;
  menu_source->Append(kIdDeleteSource, wxGetStockLabel(wxID_DELETE),
                  "Delete a data source");

  // DESTINATIONS
  auto* menu_add_dest = new wxMenu;
  menu_add_dest->Append(kIdAddUnknownDestination, "Unknown",
                          "Add an unknown destination node.");
  menu_add_dest->Append(kIdAddUnknownSource, "MDF Logger",
                          "Add an MDF log file source.");

  auto* menu_dest = new wxMenu;
  menu_dest->Append(kIdAddDestination, wxGetStockLabel(wxID_ADD),
                      menu_add_dest, "Add a destination node");
  menu_dest->Append(kIdEditDestination, wxGetStockLabel(wxID_EDIT),
                      "Change a destination node");;
  menu_dest->Append(kIdDeleteDestination, wxGetStockLabel(wxID_DELETE),
                      "Delete a destination node");
  // ABOUT
  auto* menu_about = new wxMenu;
  menu_about->Append(kIdOpenLogFile, "Open Log File");
  menu_about->Append(wxID_HELP);
  menu_about->AppendSeparator();
  menu_about->Append(wxID_ABOUT, wxGetStockLabel(wxID_ABOUT));

  auto* menu_bar = new wxMenuBar;
  menu_bar->Append(menu_project, "Project");
  menu_bar->Append(menu_env, "Environment");
  menu_bar->Append(menu_db, "Database");
  menu_bar->Append(menu_source, "Source");
  menu_bar->Append(menu_dest, "Destination");
  menu_bar->Append(menu_about, wxGetStockLabel(wxID_HELP));
  wxFrameBase::SetMenuBar(menu_bar);

  project_frame_ = new ProjectFrame(this);
  if (status_bar_ = wxDocParentFrame::CreateStatusBar();
      status_bar_ != nullptr) {
    // Log output, Status, Time.
    constexpr std::array<int, 3> status_width = {-1, 150, 120};
    status_bar_->SetFieldsCount(status_width.size(), status_width.data());

    constexpr std::array<int, 3> status_styles = {wxSB_SUNKEN, wxSB_FLAT,
                                                  wxSB_FLAT};
    status_bar_->SetStatusStyles(status_styles.size(), status_styles.data());

    status_bar_->SetStatusText("Log Message", 0);
    status_bar_->SetStatusText("Status", 1);
    status_bar_->SetStatusText("Time", 2);
  }

  timer_ = new wxTimer(this, kIdTimer);
  timer_->Start(500);
}

ProjectDocument* MainFrame::GetDocument() const {
  if (auto* doc_manager = wxDocManager::GetDocumentManager();
      doc_manager != nullptr ) {
    return wxDynamicCast(doc_manager->GetCurrentDocument(),
        ProjectDocument);
  }
  return nullptr;
}

void MainFrame::OnClose(wxCloseEvent &event) {
  timer_->Stop();
  // If the window is minimized. Do not save as last position
  if (!IsIconized()) {
    bool maximized = IsMaximized();
    wxPoint end_pos = GetPosition();
    wxSize end_size = GetSize();
    auto* app_config = wxConfig::Get();

    if (maximized) {
      app_config->Write("/MainWin/Max",maximized);
    } else {
      app_config->Write("/MainWin/X", end_pos.x);
      app_config->Write("/MainWin/Y", end_pos.y);
      app_config->Write("/MainWin/XWidth", end_size.x);
      app_config->Write("/MainWin/YWidth", end_size.y);
      app_config->Write("/MainWin/Max", maximized);
    }
  }
  event.Skip(true);
}

void MainFrame::OnAbout(wxCommandEvent&) {
  wxAboutDialogInfo info;
  info.SetName("Bus Master");
  info.SetVersion("1.0");
  info.SetDescription("Bus Master Control and Configuration GUI");

  wxArrayString devs;
  devs.push_back("Ingemar Hedvall");
  info.SetDevelopers(devs);

  info.SetCopyright("(C) 2025 Ingemar Hedvall");
  info.SetLicense("MIT License (https://opensource.org/licenses/MIT)\n"
      "Copyright 2025 Ingemar Hedvall\n"
      "\n"
      "Permission is hereby granted, free of charge, to any person obtaining a copy of this\n"
      "software and associated documentation files (the \"Software\"),\n"
      "to deal in the Software without restriction, including without limitation the rights to use, copy,\n"
      "modify, merge, publish, distribute, sublicense, and/or sell copies of the Software,\n"
      "and to permit persons to whom the Software is furnished to do so, subject to the following conditions:\n"
      "\n"
      "The above copyright notice and this permission notice shall be included in all copies or substantial\n"
      "portions of the Software.\n"
      "\n"
      "THE SOFTWARE IS PROVIDED \"AS IS\", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,\n"
      "INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR\n"
      "PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,\n"
      "DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR\n"
      "IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE."
  );
  if (!info.HasIcon()) {
#ifdef _WIN32
      wxIcon app("APP_ICON", wxBITMAP_TYPE_ICO_RESOURCE);
#else
      wxIcon app {wxICON(app)};
#endif
      info.SetIcon(app);
  }
  wxAboutBox(info);
}

void MainFrame::OnDropFiles(wxDropFilesEvent& event) {
  // Note can only add one project document file
  const int count = event.GetNumberOfFiles();
  if (count != 1) {
    wxMessageBox("Dropping the file error. Only one file can be added.");
    return;
  }

  const auto* list = event.GetFiles();
  auto* man = wxDocManager::GetDocumentManager();
  if (list == nullptr || man == nullptr)  {
    wxMessageBox("Dropping the file error. No document manager.");
    return;
  }

  const wxString config_file = list[0];
  if (const bool is_project_file =
    Project::IsProjectFile(config_file.ToStdString());
      !is_project_file ) {
    wxMessageBox("Dropping the file error. Not a project file. File; "
      + config_file);
    return;
  }

  if (const auto* doc_exist =
            man->FindDocumentByPath(config_file);
      doc_exist != nullptr) {
    return;
  }
  man->CreateDocument(config_file, wxDOC_SILENT);
}

void MainFrame::OnUpdateNoDocument(wxUpdateUIEvent& event) {
  if (const auto* doc = GetDocument();
      doc != nullptr) {
    event.Skip();
  } else {
    event.Enable(false);
  }
}

void MainFrame::OnTimer(wxTimerEvent& event) {
  // Update the timers

  if (status_bar_ != nullptr) {
    const wxString date_time = GetLocalDateTime();
    const wxString old_date_time = status_bar_->GetStatusText(2);
    if (old_date_time != date_time) {
      status_bar_->SetStatusText(date_time, 2);
    }
  }

  // ToDo: Update the status pane
}

}
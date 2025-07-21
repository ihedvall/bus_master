/*
 * Copyright 2025 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */
#include "busmaster.h"

#include <util/logconfig.h>
#include <util/logstream.h>

#include <bus/buslogstream.h>

#include <wx/config.h>
#include <wx/docview.h>
#include <wx/utils.h>
#include <wx/wx.h>

#include <boost/asio.hpp>
#include <boost/filesystem.hpp>
#include <boost/locale.hpp>
#include <boost/process.hpp>
#include <filesystem>
#include <string>
#include <vector>

#include "mainframe.h"
#include "projectdocument.h"
#include "projectview.h"
#include "windowid.h"

using namespace util::log;

wxIMPLEMENT_APP(bus::BusMaster);

namespace {
  void BusLogFunction(const std::source_location& location,
                   bus::BusLogSeverity severity,
                   const std::string& message) {
    util::log::LogMessage msg;
    msg.message = message;
    msg.severity = static_cast<util::log::LogSeverity>(severity);
    msg.location = location;
    util::log::LogConfig::Instance().AddLogMessage(msg);
  }

}

namespace bus {

wxBEGIN_EVENT_TABLE(BusMaster, wxApp)
  EVT_UPDATE_UI(kIdOpenLogFile, BusMaster::OnUpdateOpenLogFile)
  EVT_MENU(kIdOpenLogFile, BusMaster::OnOpenLogFile)
wxEND_EVENT_TABLE()

bool BusMaster::OnInit() {
  SetAppearance(Appearance::System);
  if (!wxApp::OnInit()) {
    return false;
  }

  // Setup correct localization when formatting date and times
  boost::locale::generator gen;
  std::locale::global(gen(""));

    // Setup system basic configuration
  SetVendorDisplayName("IH Development");
  SetVendorName("IH_Development");
  SetAppName("BusMaster");
  SetAppDisplayName("Bus Master");

  // Set up the log file.
  auto& log_config = LogConfig::Instance();
  log_config.Type(LogType::LogToFile);
  log_config.SubDir("bus_master/log");
  log_config.BaseName("bus_master");
  log_config.CreateDefaultLogger();

  // Add the log to list logger
  const std::vector<std::string> empty_args;
  log_config.AddLogger("ListLogger", LogType::LogToList,empty_args);

  // Redirect bus message log to this log system.
  BusLogStream::UserLogFunction = BusLogFunction;

  LOG_INFO() << "Log File created. Path: " << log_config.GetLogFile();

  notepad_ = util::log::FindNotepad();

  auto* app_config = wxConfig::Get();
  wxPoint start_pos;
  app_config->Read("/MainWin/X",&start_pos.x, wxDefaultPosition.x);
  app_config->Read("/MainWin/Y",&start_pos.y, wxDefaultPosition.x);
  wxSize start_size;
  app_config->Read("/MainWin/XWidth",&start_size.x, 1200);
  app_config->Read("/MainWin/YWidth",&start_size.y, 800);
  bool maximized = false;
  app_config->Read("/MainWin/Max",&maximized, maximized);

  auto* doc_manager = new wxDocManager;
  new wxDocTemplate(doc_manager, "Project File","*.xml;","",
                    "xml","Project Document","Project View",
                     wxCLASSINFO(ProjectDocument),
                     wxCLASSINFO(ProjectView));
  doc_manager->SetMaxDocsOpen(1);

  auto* frame = new MainFrame(doc_manager, GetAppDisplayName(), start_pos,
                              start_size, maximized);
  SetTopWindow(frame);
  frame->Show(true);
  return true;
}

int BusMaster::OnExit() {
  LOG_INFO() << "Closing app";

  auto* app_config = wxConfig::Get();
  if (auto* doc_manager = wxDocManager::GetDocumentManager();
      doc_manager != nullptr && app_config != nullptr) {
    doc_manager->FileHistorySave(*app_config);
    delete doc_manager;
  }
  LOG_INFO() << "Saved file history.";

  // Reset bus message log.
  BusLogStream::UserLogFunction = BusLogStream::BusNoLogFunction;

  auto& log_config = LogConfig::Instance();
  log_config.DeleteLogChain();
  return wxApp::OnExit();
}

void BusMaster::OnOpenLogFile(wxCommandEvent& event) {
  auto& log_config = LogConfig::Instance();
  std::string logfile = log_config.GetLogFile();
  OpenFile(logfile);

}

void BusMaster::OnUpdateOpenLogFile(wxUpdateUIEvent &event) {
  if (notepad_.empty()) {
    event.Enable(false);
    return;
  }

  auto& log_config = LogConfig::Instance();
  std::string logfile = log_config.GetLogFile();
  try {
    std::filesystem::path p(logfile);
    const bool exist = std::filesystem::exists(p);
    event.Enable(exist);
  } catch (const std::exception&) {
    event.Enable(false);
  }
}


void BusMaster::OpenFile(const std::string& filename) {
  if (!notepad_.empty()) {
    std::vector<std::string> arguments = {filename};
    boost::process::process open_log(ctx_, notepad_.c_str(), arguments);
    open_log.detach();
  }
}

}



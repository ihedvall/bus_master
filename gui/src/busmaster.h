/*
 * Copyright 2025 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */
#pragma once

#include <string>

#include <boost/asio.hpp>

#include <wx/wx.h>

namespace bus {

class BusMaster : public wxApp {
 public:

  bool OnInit() override;
  int OnExit() override;

  void OpenFile(const std::string& filename);

 private:
  boost::asio::io_context ctx_;
  std::string notepad_; ///< Path to notepad.exe if it exist

  void OnOpenLogFile(wxCommandEvent& event);
  void OnUpdateOpenLogFile(wxUpdateUIEvent& event);

  wxDECLARE_EVENT_TABLE();
};
} // bus

wxDECLARE_APP(bus::BusMaster);

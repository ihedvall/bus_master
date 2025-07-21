/*
* Copyright 2025 Ingemar Hedvall
* SPDX-License-Identifier: MIT
*/
#include <string_view>
#include <array>

#include "loglistview.h"

#include <util/logconfig.h>
#include <util/logmessage.h>
#include <util/timestamp.h>

#include "util/logstream.h"
#include "windowid.h"

using namespace util::log;
using namespace util::time;

namespace {
constexpr std::array<std::string_view, 9> kSeverityList = {
  "Trace", "Debug", "Info", "Notice",
  "Warning", "Error", "Critical", "Alert",
  "Emergency"
};

const std::string_view& SeverityToString(util::log::LogSeverity severity) {
  const size_t index = static_cast<size_t>(severity);
  if (index < kSeverityList.size()) {
    return kSeverityList[index];
  }
  return kSeverityList[0];
}

}

namespace bus {
LogListView::LogListView(wxWindow *parent)
 : wxListView(parent, kIdLogListView, wxDefaultPosition, wxSize(400, 100),
                 wxLC_REPORT | wxLC_SINGLE_SEL | wxLC_VIRTUAL),
   image_list_(16,16, false, 9) {
  auto& log_config = LogConfig::Instance();
  if (auto* logger = log_config.GetLogger("ListLogger"); logger != nullptr) {
    log_to_list_ = dynamic_cast<LogToList*>(logger);
  }
  if (log_to_list_ != nullptr) {
    log_to_list_->EnableSeverityLevel(util::log::LogSeverity::kTrace, true);
  }
  RemoveSortIndicator();
  AppendColumn("Severity", wxLIST_FORMAT_LEFT, 100);
  AppendColumn("Time", wxLIST_FORMAT_LEFT, 150);
  AppendColumn("Message", wxLIST_FORMAT_LEFT, 500);

  wxBitmap list_view("LOG_VIEW", wxBITMAP_TYPE_BMP_RESOURCE);
  image_list_.Add(list_view);
  wxListView::SetImageList(&image_list_, wxIMAGE_LIST_SMALL);
}

wxItemAttr* LogListView::OnGetItemAttr(long item) const {
  return wxListView::OnGetItemAttr(item);
}

wxItemAttr* LogListView::OnGetItemColumnAttr(long item, long column) const {
  return wxListCtrlBase::OnGetItemColumnAttr(item, column);
}

int LogListView::OnGetItemColumnImage(long item, long column) const {
  if (item < 0 || log_to_list_ == nullptr || column != 0) {
    return -1;
  }
  const LogMessage msg = log_to_list_->GetLogMessage(item);
  return static_cast<int>(msg.severity);
}

int LogListView::OnGetItemImage(long item) const {
  if (item < 0 || log_to_list_ == nullptr) {
    return -1;
  }
  const LogMessage msg = log_to_list_->GetLogMessage(item);
  return static_cast<int>(msg.severity);
}

wxString LogListView::OnGetItemText(long item, long column) const {
  wxString text;
  if (item < 0 || column < 0 || log_to_list_ == nullptr) {
    return text;
  }
  const LogMessage msg = log_to_list_->GetLogMessage(item);
  switch (column) {
    case 0:
      text = wxString::FromUTF8(SeverityToString(msg.severity) );
      break;

    case 1:
     text = wxString::FromUTF8( GetLocalDateTime(msg.timestamp));
     break;

    case 2:
      text = wxString::FromUTF8(msg.message);
      break;

    default:
      break;

  }

  return text;
}

void LogListView::Update() {
  if (log_to_list_ != nullptr) {
    SetItemCount(log_to_list_->Size());
    Refresh();
  }
}

void LogListView::CheckLogView() {
  if (log_to_list_ != nullptr) {
    const size_t change_number = log_to_list_->ChangeNumber();
    if (old_change_number_ != change_number) {
      SetItemCount(log_to_list_->Size());
      Refresh();
      old_change_number_ = change_number;
    }
  }
}

}  // namespace bus
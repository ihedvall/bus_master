/*
* Copyright 2025 Ingemar Hedvall
* SPDX-License-Identifier: MIT
 */
#include "messagelistview.h"

#include <array>
#include <string_view>

#include "util/logstream.h"
#include "windowid.h"

using namespace util::log;

namespace {

}

namespace bus {
MessageListView::MessageListView(wxWindow *parent)
    : wxListView(parent, kIdLogListView, wxDefaultPosition, wxSize(400, 100),
                 wxLC_REPORT | wxLC_SINGLE_SEL | wxLC_VIRTUAL) {
  RemoveSortIndicator();
  AppendColumn("Time [s]", wxLIST_FORMAT_LEFT, 150);
  AppendColumn("Type", wxLIST_FORMAT_LEFT, 150);
  AppendColumn("ID", wxLIST_FORMAT_LEFT, 100);
  AppendColumn("Data", wxLIST_FORMAT_LEFT, 150);
  AppendColumn("Flags", wxLIST_FORMAT_LEFT, 100);
}

wxItemAttr* MessageListView::OnGetItemAttr(long item) const {
  return wxListView::OnGetItemAttr(item);
}

wxItemAttr* MessageListView::OnGetItemColumnAttr(long item, long column) const {
  return wxListCtrlBase::OnGetItemColumnAttr(item, column);
}

int MessageListView::OnGetItemColumnImage(long item, long column) const {
  return -1;
}

int MessageListView::OnGetItemImage(long item) const {
  return -1;
}

wxString MessageListView::OnGetItemText(long item, long column) const {
  wxString text;
  if (item < 0 || column < 0 || source_ == nullptr) {
    return text;
  }

  const IBusMessage* msg = source_->GetMessage(item);
  if (msg == nullptr) {
    return text;
  }

  switch (column) {
    case 0: {  // Time column
      const int64_t start_time = static_cast<int64_t>(source_->FirstTime());
      int64_t temp = static_cast<int64_t>(msg->Timestamp()) - start_time;
      double time = static_cast<double>(temp) / 1'000'000'000;
      text = wxString::FromDouble(time);
      break;
    }

    case 1:
      text = "Data Frame";
      break;

    default:
      break;

  }

  return text;
}

void MessageListView::Update() {
  if (source_ != nullptr) {
    SetItemCount(source_->NofMessages());
    Refresh();
  }
}

void MessageListView::CheckMessageView() {

}

}  // namespace bus
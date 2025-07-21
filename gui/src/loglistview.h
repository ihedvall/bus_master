/*
* Copyright 2025 Ingemar Hedvall
* SPDX-License-Identifier: MIT
*/

#pragma once

#include <wx/listctrl.h>
#include <wx/imaglist.h>
#include <util/logtolist.h>

namespace bus {

class LogListView : public wxListView {
  public:
   explicit LogListView(wxWindow *parent);

   void CheckLogView();
   void Update() override;
  protected:
   [[nodiscard]] wxItemAttr* OnGetItemAttr(long item) const override;
   [[nodiscard]] wxItemAttr* OnGetItemColumnAttr(long item, long column) const override;
   [[nodiscard]] int OnGetItemColumnImage(long item, long column) const override;
   [[nodiscard]] int OnGetItemImage(long item) const override;
   [[nodiscard]] wxString OnGetItemText(long item, long column) const override;

  private:
   util::log::LogToList* log_to_list_ = nullptr;
   size_t old_change_number_ = 0;
   wxImageList image_list_;
};

}  // namespace bus


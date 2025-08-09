/*
* Copyright 2025 Ingemar Hedvall
* SPDX-License-Identifier: MIT
 */

#pragma once

#include <wx/listctrl.h>
#include <wx/imaglist.h>
#include "bus/mdftrafficgenerator.h"

namespace bus {

class MessageListView : public wxListView {
 public:
  explicit MessageListView(wxWindow *parent);

  void SetSource(MdfTrafficGenerator* source) { source_ = source;}

  void CheckMessageView();
  void Update() override;
 protected:
  [[nodiscard]] wxItemAttr* OnGetItemAttr(long item) const override;
  [[nodiscard]] wxItemAttr* OnGetItemColumnAttr(long item, long column) const override;
  [[nodiscard]] int OnGetItemColumnImage(long item, long column) const override;
  [[nodiscard]] int OnGetItemImage(long item) const override;
  [[nodiscard]] wxString OnGetItemText(long item, long column) const override;

 private:
  MdfTrafficGenerator* source_ = nullptr;
};

}  // namespace bus


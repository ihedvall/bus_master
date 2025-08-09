/*
* Copyright 2025 Ingemar Hedvall
* SPDX-License-Identifier: MIT
*/

#pragma once

#include <deque>
#include <memory>

#include "bus/isource.h"

#include <mdf/canmessage.h>
#include <bus/ibusmessage.h>

namespace bus {

class MdfTrafficGenerator : public ISource {
 public:
  MdfTrafficGenerator();
  void Enable(bool enable) override;
  uint64_t FirstTime() const;
  const IBusMessage* GetMessage(size_t index) const;

  size_t NofMessages() const { return message_list_.size(); }
 private:
  uint64_t start_time_ = 0;


  std::deque<std::unique_ptr<IBusMessage>> message_list_;

  [[nodiscard]] bool CheckMdfFile();
  [[nodiscard]] bool ReadMdfFile();

  [[nodiscard]] bool CanMessageCallback(uint64_t sample,
                                        const mdf::CanMessage& msg);
  static bool LessTime(const std::unique_ptr<IBusMessage>& msg1,
                          const std::unique_ptr<IBusMessage>& msg2);
  void AddCanMessage(const mdf::CanMessage& msg);
};

}  // namespace bus


/*
* Copyright 2025 Ingemar Hedvall
* SPDX-License-Identifier: MIT
 */

#pragma once

#include <cstdint>
#include <string>

#include <metric/metric.h>

namespace bus {

class DbMetric : public metric::Metric {
 public:
  void GroupName(std::string name) {group_name_ = name;}
  [[nodiscard]] const std::string& GroupName() const { return group_name_; }

  void GroupId(uint32_t identity) { group_id_ = identity; }
  [[nodiscard]] uint32_t GroupId() { return group_id_; }

 private:
  std::string group_name_;
  uint32_t group_id_ = 0;
};

}  // namespace bus


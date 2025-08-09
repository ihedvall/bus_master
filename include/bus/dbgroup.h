/*
* Copyright 2025 Ingemar Hedvall
* SPDX-License-Identifier: MIT
 */

#pragma once

#include <cstdint>
#include <string>
#include <algorithm>

namespace bus {

enum class TypeOfDbGroup : int {
  General = 0,
  CanMessage = 1
};

class DbGroup {
 public:
  void Name(std::string name) { name_ = std::move(name); }
  [[nodiscard]] const std::string& Name() const { return name_; }

  void Description(std::string desc) { description_ = std::move(desc); }
  [[nodiscard]] const std::string& Description() const { return description_; }

  void Type(TypeOfDbGroup type) { type_ = type; }
  [[nodiscard]] TypeOfDbGroup Type() const { return type_; }

  void Identity(uint32_t identity) { identity_ = identity; }
  [[nodiscard]] uint32_t Identity() const { return identity_; }

 private:
  std::string name_;
  std::string description_;
  TypeOfDbGroup type_ = TypeOfDbGroup::General;
  uint32_t identity_ = 0;

};

}  // namespace bus


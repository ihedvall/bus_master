/*
 * Copyright 2025 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */

 #pragma once

#include <string>

namespace bus {

enum class BusPropertyType {
  NormalItem = 0,
  HeaderItem = 1,
  BlankItem = 3
};

class BusProperty {
 public:
  BusProperty() = default;                   ///< Blank item
  explicit BusProperty(std::string header);  ///< Header item.

  /** \brief Normal item. */
  BusProperty(std::string label, std::string value, std::string unit = {},
              BusPropertyType type = BusPropertyType::NormalItem);

  [[nodiscard]] const std::string& Label() const { return label_; }

  [[nodiscard]] const std::string& Value() const { return value_; }

  [[nodiscard]] const std::string& Unit() const { return unit_; }

  [[nodiscard]] BusPropertyType Type() const { return type_; }

 private:
  std::string label_;
  std::string value_;
  std::string unit_;
  BusPropertyType type_ = BusPropertyType::BlankItem;
};


} // bus


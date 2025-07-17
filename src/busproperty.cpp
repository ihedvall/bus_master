/*
* Copyright 2025 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */

#include "bus/busproperty.h"

namespace bus {
BusProperty::BusProperty(std::string header)
    : label_(std::move(header)), type_(BusPropertyType::HeaderItem) {}

BusProperty::BusProperty(std::string label, std::string value, std::string unit,
                         BusPropertyType type)
: label_(std::move(label)),
  value_(std::move(value)),
  unit_(std::move(unit)),
  type_(type) {

}

} // bus
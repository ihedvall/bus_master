/*
* Copyright 2025 Ingemar Hedvall
* SPDX-License-Identifier: MIT
 */
#include "bus/idestination.h"

#include <array>
#include <string>

#include "util/ixmlnode.h"
#include "util/stringutil.h"

using namespace util::xml;
using namespace util::string;

namespace {

constexpr std::array<std::string_view, 2> kTypeList =
    { "Unknown", "MDF Bus Logger" };
}

namespace bus {

IDestination& IDestination::operator=(const IDestination& destination) {
  if (this == &destination) {
    return *this;
  }
  name_ = destination.name_;
  description_ = destination.description_;
  // Not copying the type and all the dynamic properties.
  return *this;
}

void IDestination::Start() {
  if (enabled_) {
    started_ = true;
    operable_ = true;
  } else {
    started_ = false;
    operable_ = false;
  }
}

void IDestination::Stop() {
  started_ = false;
  operable_ = false;
}

void IDestination::WriteConfig(IXmlNode& root_node) const {
  auto& dest_node = root_node.AddNode("Destination");
  dest_node.SetAttribute("name", name_);
  dest_node.SetAttribute("type", TypeToString(type_));
  dest_node.SetProperty("Type", TypeToString(type_));
  dest_node.SetProperty("Name", name_);
  dest_node.SetProperty("Description", description_);
}

void IDestination::ReadConfig(const IXmlNode& dest_node) {
  name_ = dest_node.Attribute<std::string>("name");
  if (name_.empty()) {
    name_ = dest_node.Property<std::string>("Name");
  }
  // Note that the project reads the type property as its need to create the
  // right environment type.
  description_ = dest_node.Property<std::string>("Description");
}

std::string_view IDestination::TypeToString(TypeOfDestination type) {
  for (size_t index = 0; index < kTypeList.size(); ++index) {
    const auto& dest_type = static_cast<TypeOfDestination>(index);
    if (dest_type == type) {
      return kTypeList[index];
    }
  }
  return kTypeList[0];
}

TypeOfDestination IDestination::TypeFromString(const std::string& type) {
  for (size_t index = 0; index < kTypeList.size(); ++index) {
    const auto& dest_type = static_cast<TypeOfDestination>(index);
    if (IEquals(std::string(kTypeList[index]),type) ) {
      return dest_type;
    }
  }
  return TypeOfDestination::Unknown;
}

void IDestination::ToProperties(std::vector<BusProperty>& properties) const {
  if (!properties.empty()) {
    properties.emplace_back();
  }
  properties.emplace_back("Source");
  properties.emplace_back("Type", std::string(TypeToString(type_)));
  properties.emplace_back("Name", Name());
  properties.emplace_back();
  properties.emplace_back("Status");
  properties.emplace_back("Enabled", enabled_ ? "Yes" : "No");
  properties.emplace_back("State", started_ ?
                                            (operable_ ? "Running" : "Failing") : "Stopped");
  properties.emplace_back("Operable", operable_ ? "Yes" : "No");
}

} // bus
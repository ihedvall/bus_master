/*
* Copyright 2025 Ingemar Hedvall
* SPDX-License-Identifier: MIT
*/
#include <cstdint>
#include <string>
#include <array>

#include <util/ixmlnode.h>
#include <util/stringutil.h>

#include "bus/idatabase.h"

using namespace util::xml;
using namespace util::string;

namespace {
constexpr std::array<std::string_view, 4> kTypeList =
    { "Unknown", "SQLite Database", "DBC File", "A2L File" };
}
namespace bus {

IDatabase& IDatabase::operator=(const IDatabase& db) {
  if (this == &db) {
    return *this;
  }
  name_ = db.name_;
  description_ = db.description_;
  filename_ = db.filename_;

  // Not copying the type and all the dynamic properties.
  return *this;
}

void IDatabase::Activate() {
  active_ = true;
  operable_ = true;
}

void IDatabase::Deactivate() {
  active_ = false;
  operable_ = false;
}

void IDatabase::WriteConfig(IXmlNode& root_node) const {
  auto& env_node = root_node.AddNode("Database");
  env_node.SetAttribute("name", name_);
  env_node.SetAttribute("type", TypeToString(type_));
  env_node.SetProperty("Type", TypeToString(type_));
  env_node.SetProperty("Name", name_);
  env_node.SetProperty("Description", description_);
  env_node.SetProperty("Filename", filename_);
}

void IDatabase::ReadConfig(const IXmlNode& env_node) {
  name_ = env_node.Attribute<std::string>("name");
  if (name_.empty()) {
    name_ = env_node.Property<std::string>("Name");
  }
  // Note that the project reads the type property as its need to create the
  // right environment type.
  description_ = env_node.Property<std::string>("Description");
  filename_ = env_node.Property<std::string>("Filename");
}

std::string_view IDatabase::TypeToString(TypeOfDatabase type) {
  for (size_t index = 0; index < kTypeList.size(); ++index) {
    const auto& env_type = static_cast<TypeOfDatabase>(index);
    if (env_type == type) {
      return kTypeList[index];
    }
  }
  return kTypeList[0];
}

TypeOfDatabase IDatabase::TypeFromString(const std::string& type) {
  for (size_t index = 0; index < kTypeList.size(); ++index) {
    const auto& env_type = static_cast<TypeOfDatabase>(index);
    if (IEquals(std::string(kTypeList[index]),type) ) {
      return env_type;
    }
  }
  return TypeOfDatabase::Unknown;
}

void IDatabase::ToProperties(std::vector<BusProperty>& properties) const {
  if (!properties.empty()) {
    properties.emplace_back();
  }
  properties.emplace_back("Environment");
  properties.emplace_back("Type", std::string(TypeToString(type_)));
  properties.emplace_back("Name", Name());
  properties.emplace_back("Description", Description());
  properties.emplace_back("Filename", Filename());

  properties.emplace_back();
  properties.emplace_back("Status");
  properties.emplace_back("Active", active_ ? (operable_ ? "Active" : "Failing") : "Inactive");
  properties.emplace_back("Operable", operable_ ? "Yes" : "No");
}
}  // namespace bus
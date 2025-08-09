/*
* Copyright 2025 Ingemar Hedvall
* SPDX-License-Identifier: MIT
*/
#include <cstdint>
#include <string>
#include <ranges>
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

void IDatabase::Enable(bool enable) {
  enabled_ = enable;
  operable_ = enable;
}

void IDatabase::WriteConfig(IXmlNode& root_node) const {
  auto& db_node = root_node.AddNode("Database");
  db_node.SetAttribute("name", name_);
  db_node.SetAttribute("type", TypeToString(type_));
  db_node.SetProperty("Type", TypeToString(type_));
  db_node.SetProperty("Name", name_);
  db_node.SetProperty("Description", description_);
  db_node.SetProperty("Filename", filename_);
  db_node.SetProperty("Enabled", IsEnabled());
}

void IDatabase::ReadConfig(const IXmlNode& db_node) {
  name_ = db_node.Attribute<std::string>("name");
  if (name_.empty()) {
    name_ = db_node.Property<std::string>("Name");
  }
  // Note that the project reads the type property as its need to create the
  // right environment type.
  description_ = db_node.Property<std::string>("Description");
  filename_ = db_node.Property<std::string>("Filename");
  enabled_ = db_node.Property<bool>("Enabled");
}

std::string_view IDatabase::TypeToString(TypeOfDatabase type) {
  for (size_t index = 0; index < kTypeList.size(); ++index) {
    const auto db_type = static_cast<TypeOfDatabase>(index);
    if (db_type == type) {
      return kTypeList[index];
    }
  }
  return kTypeList[0];
}

TypeOfDatabase IDatabase::TypeFromString(const std::string& type) {
  for (size_t index = 0; index < kTypeList.size(); ++index) {
    const auto db_type = static_cast<TypeOfDatabase>(index);
    if (IEquals(std::string(kTypeList[index]),type) ) {
      return db_type;
    }
  }
  return TypeOfDatabase::Unknown;
}

void IDatabase::ToProperties(std::vector<BusProperty>& properties) const {
  if (!properties.empty()) {
    properties.emplace_back();
  }
  properties.emplace_back("Database");
  properties.emplace_back("Type", std::string(TypeToString(type_)));
  properties.emplace_back("Name", Name());
  properties.emplace_back("Description", Description());
  properties.emplace_back("Filename", Filename());
  properties.emplace_back("Nof Groups", std::to_string(Groups().size()));
  properties.emplace_back("Nof Metrics", std::to_string(Metrics().size()));

  properties.emplace_back();
  properties.emplace_back("Status");
  properties.emplace_back("Enabled", enabled_ ? (enabled_ ? "Enabled" : "Failing") : "Disabled");
  properties.emplace_back("Operable", operable_ ? "Yes" : "No");
}

void IDatabase::ParseMessage(const IBusMessage& message) {}

DbGroup* IDatabase::CreateGroup(std::string name, uint32_t identity) {
  auto itr = std::ranges::find_if( group_list_, [&] (const auto& group) -> bool {
    return group && group->Name() == name && group->Identity() == identity;
  });
  if (itr == group_list_.end()) {
    if (auto new_group = std::make_unique<DbGroup>(); new_group ) {
      new_group->Name(std::move(name));
      new_group->Identity(identity);
      group_list_.emplace_back(std::move(new_group));
    } else {
      return itr->get();
    }
  }
  return group_list_.back().get();
}

void IDatabase::DeleteGroup(std::string name, uint32_t identity) {
  std::erase_if(group_list_, [&] (const auto& group) -> bool {
    return !group || (group->Name() == name && group->Identity() == identity);
  });
}

DbMetric* IDatabase::CreateMetric(const DbGroup& group, std::string name) {
  auto itr = std::ranges::find_if( metric_list_, [&] (const auto& metric) -> bool {
    return metric && group.Name() == metric->GroupName()
           && group.Identity() == metric->GroupId() && metric->Name() == name;
  });
  if (itr == metric_list_.end()) {
    if (auto new_metric = std::make_unique<DbMetric>(); new_metric ) {
      new_metric->Name(std::move(name));
      new_metric->GroupName(group.Name());
      new_metric->GroupId(group.Identity());
      metric_list_.emplace_back(std::move(new_metric));
    } else {
      return itr->get();
    }
  }
  return metric_list_.back().get();
}

void IDatabase::DeleteMetric(const DbGroup& group, std::string name) {
  std::erase_if(metric_list_, [&] (const auto& metric) -> bool {
    return !metric ||
           (metric->GroupName() == group.Name()
            && metric->GroupId() == group.Identity()
            && metric->Name() == name);
  });
}

}  // namespace bus
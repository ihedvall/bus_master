/*
* Copyright 2025 Ingemar Hedvall
* SPDX-License-Identifier: MIT
*/
#include <string>
#include <array>

#include <util/ixmlnode.h>
#include <util/stringutil.h>

#include "bus/isource.h"

using namespace util::xml;
using namespace util::string;

namespace {

constexpr std::array<std::string_view, 2> kTypeList =
   { "Unknown", "MDF Traffic Generator" };
}

namespace bus {

ISource& ISource::operator=(const ISource& source) {
 if (this == &source) {
   return *this;
 }
 name_ = source.name_;
 description_ = source.description_;
 // Not copying the type and all the dynamic properties.
 return *this;
}
void ISource::Start() {
  if (enabled_) {
    started_ = true;
    operable_ = true;
  } else {
    started_ = false;
    operable_ = false;
  }
}

void ISource::Stop() {
 started_ = false;
 operable_ = false;
}

void ISource::WriteConfig(IXmlNode& root_node) const {
 auto& source_node = root_node.AddNode("Source");
 source_node.SetAttribute("name", name_);
 source_node.SetAttribute("type", TypeToString(type_));
 source_node.SetProperty("Type", TypeToString(type_));
 source_node.SetProperty("Name", name_);
 source_node.SetProperty("Description", description_);
}

void ISource::ReadConfig(const IXmlNode& source_node) {
 name_ = source_node.Attribute<std::string>("name");
 if (name_.empty()) {
   name_ = source_node.Property<std::string>("Name");
 }
 // Note that the project reads the type property as its need to create the
 // right environment type.
 description_ = source_node.Property<std::string>("Description");
}

std::string_view ISource::TypeToString(TypeOfSource type) {
 for (size_t index = 0; index < kTypeList.size(); ++index) {
   const auto& source_type = static_cast<TypeOfSource>(index);
   if (source_type == type) {
     return kTypeList[index];
   }
 }
 return kTypeList[0];
}

TypeOfSource ISource::TypeFromString(const std::string& type) {
 for (size_t index = 0; index < kTypeList.size(); ++index) {
   const auto& env_type = static_cast<TypeOfSource>(index);
   if (IEquals(std::string(kTypeList[index]),type) ) {
     return env_type;
   }
 }
 return TypeOfSource::Unknown;
}

void ISource::ToProperties(std::vector<BusProperty>& properties) const {
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
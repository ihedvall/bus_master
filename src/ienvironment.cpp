/*
 * Copyright 2025 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */
#include <cstdint>
#include <string>
#include <array>


#include <util/ixmlnode.h>
#include <util/stringutil.h>

#include "bus/ienvironment.h"

using namespace util::xml;
using namespace util::string;

namespace {
  constexpr std::array<std::string_view, 3> kTypeList =
    { "Dummy", "Supervise Master", "Broker Rx Main" };
}

namespace bus {

IEnvironment& IEnvironment::operator=(const IEnvironment& env) {
  if (this == &env) {
    return *this;
  }
  name_ = env.name_;
  description_ = env.description_;
  config_file_ = env.config_file_;
  shared_memory_name_ = env.shared_memory_name_;
  host_name_ = env.host_name_;
  port_ = env.port_;
  // Not copying the type and all the dynamic properties.
  return *this;
}
void IEnvironment::Start() {
  started_ = true;
  operable_ = true;
}

void IEnvironment::Stop() {
  started_ = false;
  operable_ = false;
}

void IEnvironment::WriteConfig(IXmlNode& root_node) const {
   auto& env_node = root_node.AddNode("Environment");
   env_node.SetAttribute("name", name_);
   env_node.SetAttribute("type", TypeToString(type_));
   env_node.SetProperty("Type", TypeToString(type_));
   env_node.SetProperty("Name", name_);
   env_node.SetProperty("Description", description_);
   env_node.SetProperty("ConfigFile", config_file_);
   env_node.SetProperty("Enabled", enabled_);
   env_node.SetProperty("SharedMemoryName", shared_memory_name_);
   env_node.SetProperty("HostName", host_name_);
   env_node.SetProperty("Port", port_);
}

void IEnvironment::ReadConfig(const IXmlNode& env_node) {
  name_ = env_node.Attribute<std::string>("name");
  if (name_.empty()) {
    name_ = env_node.Property<std::string>("Name");
  }
  // Note that the project reads the type property as its need to create the
  // right environment type.
  description_ = env_node.Property<std::string>("Description");
  config_file_ = env_node.Property<std::string>("ConfigFile");
  enabled_ = env_node.Property<bool>("Enabled", true);
  shared_memory_name_ = env_node.Property<std::string>("SharedMemoryName");
  host_name_ = env_node.Property<std::string>("HostName", "127.0.0.1");
  port_ = env_node.Property<uint16_t>("Port", 43611);
}

std::string_view IEnvironment::TypeToString(TypeOfEnvironment type) {
  for (size_t index = 0; index < kTypeList.size(); ++index) {
    const auto env_type = static_cast<TypeOfEnvironment>(index);
    if (env_type == type) {
      return kTypeList[index];
    }
  }
  return kTypeList[0];
}

TypeOfEnvironment IEnvironment::TypeFromString(const std::string& type) {
  for (size_t index = 0; index < kTypeList.size(); ++index) {
    const auto env_type = static_cast<TypeOfEnvironment>(index);
    if (IEquals(std::string(kTypeList[index]),type) ) {
      return env_type;
    }
  }
  return TypeOfEnvironment::DummyEnvironment;
}

void IEnvironment::ToProperties(std::vector<BusProperty>& properties) const {
  if (!properties.empty()) {
    properties.emplace_back();
  }
  properties.emplace_back("Environment");
  properties.emplace_back("Type", std::string(TypeToString(type_)));
  properties.emplace_back("Name", Name());
  properties.emplace_back("Description", Description());
  properties.emplace_back("Configuration File", ConfigFile());
  properties.emplace_back("Enabled", IsEnabled() ? "Yes" : "No");
  properties.emplace_back("Shared Memory Name", SharedMemoryName());
  properties.emplace_back("Host Name", HostName());
  properties.emplace_back("TCP/IP Port", std::to_string(port_));
  properties.emplace_back();
  properties.emplace_back("Status");
  properties.emplace_back("State",
             started_ ? (operable_ ? "Running" : "Failing") : "Stopped");
  properties.emplace_back("Operable", operable_ ? "Yes" : "No");
}

} // bus
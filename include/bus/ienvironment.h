/*
 * Copyright 2025 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <algorithm>
#include <atomic>
#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

#include "bus/busproperty.h"

namespace util::xml {
class IXmlNode;
}

namespace bus {

enum class TypeOfEnvironment : int {
  DummyEnvironment = 0,
  SuperviseMasterEnvironment = 1
};

class IEnvironment {
 public:
  IEnvironment() = default;
  virtual ~IEnvironment() = default;
  IEnvironment& operator = (const IEnvironment& env);
  [[nodiscard]] TypeOfEnvironment Type() const {return type_; };

  static TypeOfEnvironment TypeFromString(const std::string& type);
  static std::string_view TypeToString(TypeOfEnvironment type);

  void Name(std::string name) { name_ = std::move(name); }
  [[nodiscard]] const std::string& Name() const { return name_; }

  void Description(std::string desc) { description_ = std::move(desc); }
  [[nodiscard]] const std::string& Description() const { return description_; }

  void ConfigFile(std::string config_file) {
    config_file_ = std::move(config_file);
  }
  [[nodiscard]] const std::string& ConfigFile() const { return config_file_; }

  void SharedMemoryName(std::string name) {
    shared_memory_name_ = std::move(name);
  }

  [[nodiscard]] const std::string& SharedMemoryName() const {
    return shared_memory_name_;
  }

  void HostName(std::string name) {host_name_ = std::move(name); }
  [[nodiscard]] const std::string& HostName() const { return host_name_; }

  void Port(uint16_t port) {port_ = port; }
  [[nodiscard]] uint16_t Port() const { return port_; }

  [[nodiscard]] virtual bool IsStarted() const {return started_; }
  [[nodiscard]] virtual bool IsOperable() const {return operable_; }

  virtual void Start();
  virtual void Stop();

  void WriteConfig(util::xml::IXmlNode& root_node) const;
  void ReadConfig(const util::xml::IXmlNode& env_node);

  void ToProperties(std::vector<BusProperty>& properties) const;
 protected:
  TypeOfEnvironment type_ = TypeOfEnvironment::DummyEnvironment;

  std::atomic<bool> started_ = false;
  mutable std::atomic<bool> operable_ = false;

 private:
  std::string name_;
  std::string description_;
  std::string config_file_;

  // ToDo: Generate and store these properties
  std::string shared_memory_name_;
  std::string host_name_ = "127.0.0.1";
  uint16_t port_ = 43611;
};

} // bus



/*
* Copyright 2025 Ingemar Hedvall
* SPDX-License-Identifier: MIT
 */

#pragma once

#include <string>
#include <string_view>
#include <vector>
#include <atomic>

#include "bus/busproperty.h"

namespace util::xml {
class IXmlNode;
}

namespace bus {
enum class TypeOfDestination : int {
  Unknown = 0,
  Mdf = 1,
};


class IDestination {
 public:
  IDestination() = default;
  virtual ~IDestination() = default;
  IDestination& operator = (const IDestination& destination);

  [[nodiscard]] TypeOfDestination Type() const {return type_; };

  static TypeOfDestination TypeFromString(const std::string& type);
  static std::string_view TypeToString(TypeOfDestination type);

  void Name(std::string name) { name_ = std::move(name); }
  [[nodiscard]] const std::string& Name() const { return name_; }

  void Description(std::string desc) { description_ = std::move(desc); }
  [[nodiscard]] const std::string& Description() const { return description_; }

  void Enable(bool enable) { enabled_ = enable; }
  [[nodiscard]] bool IsEnabled() const { return enabled_; }

  [[nodiscard]] virtual bool IsStarted() const {return started_; }
  [[nodiscard]] virtual bool IsOperable() const {return operable_; }

  virtual void Start();
  virtual void Stop();

  virtual void WriteConfig(util::xml::IXmlNode& root_node) const;
  virtual void ReadConfig(const util::xml::IXmlNode& source_node);

  virtual void ToProperties(std::vector<BusProperty>& properties) const;
 protected:
  TypeOfDestination type_ = TypeOfDestination::Unknown;
  bool enabled_ = true;
  std::atomic<bool> started_ = false;
  mutable std::atomic<bool> operable_ = false;

 private:
  std::string name_;
  std::string description_;
};

}  // namespace bus

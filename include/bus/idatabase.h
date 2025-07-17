/*
* Copyright 2025 Ingemar Hedvall
* SPDX-License-Identifier: MIT
*/

#pragma once

#include <string>
#include <string_view>
#include <memory>

#include "bus/busproperty.h"

namespace util::xml {
class IXmlNode;
}

namespace bus {

enum class TypeOfDatabase : int {
  Unknown = 0,
  Sqlite = 1,
  DbcFile = 2,
  A2LFile = 3,
};

class IDatabase {
 public:
  IDatabase() = default;
  virtual ~IDatabase() = default;
  IDatabase& operator = (const IDatabase& db);
  [[nodiscard]] TypeOfDatabase Type() const { return type_; }

  static TypeOfDatabase TypeFromString(const std::string& type);
  static std::string_view TypeToString(TypeOfDatabase type);

  void Name(std::string name) { name_ = std::move(name); }
  [[nodiscard]] const std::string& Name() const { return name_; }

  void Description(std::string desc) { description_ = std::move(desc); }
  [[nodiscard]] const std::string& Description() const { return description_; }

  void Filename(std::string filename) { filename_ = std::move(filename); }
  [[nodiscard]] const std::string& Filename() const { return filename_; }

  [[nodiscard]] const std::string_view& FileFilter() const {
    return file_filter_;
  }

  [[nodiscard]] const std::string_view& DefaultExtension() const {
    return default_extension_;
  }

  virtual void Activate();
  virtual void Deactivate();

  [[nodiscard]] virtual bool IsActive() const {return active_; }
  [[nodiscard]] virtual bool IsOperable() const {return operable_; }

  void WriteConfig(util::xml::IXmlNode& root_node) const;
  void ReadConfig(const util::xml::IXmlNode& env_node);

  void ToProperties(std::vector<BusProperty>& properties) const;
 protected:
  std::atomic<bool> active_ = false;
  std::atomic<bool> operable_ = false;
  TypeOfDatabase type_ = TypeOfDatabase::Unknown;
  std::string_view file_filter_ = "All files (*.*)|*.*";
  std::string_view default_extension_ = "*.*";
 private:
  std::string name_;
  std::string description_;

  std::string filename_;
};

}  // namespace bus



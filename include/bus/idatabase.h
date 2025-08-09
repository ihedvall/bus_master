/*
* Copyright 2025 Ingemar Hedvall
* SPDX-License-Identifier: MIT
*/

#pragma once

#include <string>
#include <string_view>
#include <memory>
#include <vector>

#include "bus/busproperty.h"
#include "bus/dbgroup.h"
#include "bus/dbmetric.h"

namespace util::xml {
class IXmlNode;
}

namespace bus {

class IBusMessage;

enum class TypeOfDatabase : int {
  Unknown = 0,
  Sqlite = 1,
  DbcFile = 2,
  A2lFile = 3,
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

  virtual void Enable(bool enable);

  [[nodiscard]] virtual bool IsEnabled() const {return enabled_; }
  [[nodiscard]] virtual bool IsOperable() const {return operable_; }

  void WriteConfig(util::xml::IXmlNode& root_node) const;
  void ReadConfig(const util::xml::IXmlNode& db_node);

  void ToProperties(std::vector<BusProperty>& properties) const;

  virtual void ParseMessage(const IBusMessage& message);

  virtual DbGroup* CreateGroup(std::string name, uint32_t identity);
  void DeleteGroup(std::string name, uint32_t identity);
  const std::vector<std::unique_ptr<DbGroup>>& Groups() const {
    return group_list_;
  }

  virtual DbMetric* CreateMetric(const DbGroup& group, std::string name);
  void DeleteMetric(const DbGroup& group, std::string name);
  const std::vector<std::unique_ptr<DbMetric>>& Metrics() const {
    return metric_list_;
  }

 protected:
  std::atomic<bool> enabled_ = false;
  std::atomic<bool> operable_ = false;
  TypeOfDatabase type_ = TypeOfDatabase::Unknown;

  std::vector<std::unique_ptr<DbGroup>> group_list_;
  std::vector<std::unique_ptr<DbMetric>> metric_list_;
 private:
  std::string name_;
  std::string description_;

  std::string filename_;
};

}  // namespace bus



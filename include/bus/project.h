/*
* Copyright 2025 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <algorithm>
#include <memory>
#include <string>
#include <vector>

#include "bus/busproperty.h"
#include "bus/ienvironment.h"
#include "bus/idatabase.h"
#include "bus/isource.h"
#include "bus/idestination.h"

namespace bus {

class Project {
public:
  Project() = default;

  static bool IsProjectFile(const std::string& filename);

  void Name(std::string name) { name_ = std::move(name); }
  [[nodiscard]] const std::string& Name() const { return name_; }

  void Description(std::string desc) { description_ = std::move(desc); }
  [[nodiscard]] const std::string& Description() const { return description_; }

  void ConfigFile(std::string config_file) {
    config_file_ = std::move(config_file);
  }

  [[nodiscard]] const std::string& ConfigFile() const { return config_file_; }

  IEnvironment* CreateEnvironment(TypeOfEnvironment type);
  IEnvironment* GetEnvironment(const std::string& name) const;
  void DeleteEnvironment(std::string name);

  [[nodiscard]] const std::vector<std::unique_ptr<IEnvironment>>&
            Environments() const;

  [[nodiscard]] std::vector<std::unique_ptr<IEnvironment>>&
            Environments();

  IDatabase* CreateDatabase(TypeOfDatabase type);
  IDatabase* GetDatabase(const std::string& name) const;
  void DeleteDatabase(std::string name);

  [[nodiscard]] const std::vector<std::unique_ptr<IDatabase>>&
  Databases() const;

  [[nodiscard]] std::vector<std::unique_ptr<IDatabase>>&  Databases();

  ISource* CreateSource(TypeOfSource type);
  ISource* GetSource(const std::string& name) const;
  void DeleteSource(std::string name);

  [[nodiscard]] const std::vector<std::unique_ptr<ISource>>& Sources() const;
  [[nodiscard]] std::vector<std::unique_ptr<ISource>>& Sources();

  IDestination* CreateDestination(TypeOfDestination type);
  IDestination* GetDestination(const std::string& name) const;
  void DeleteDestination(std::string name);

  [[nodiscard]] const std::vector<std::unique_ptr<IDestination>>& Destinations() const;
  [[nodiscard]] std::vector<std::unique_ptr<IDestination>>& Destinations();

  void ToProperties(std::vector<BusProperty>& properties) const;

  [[nodiscard]] bool ReadConfig();
  [[nodiscard]] bool WriteConfig();

private:
  std::string name_;
  std::string description_;
  std::string config_file_;

  std::vector<std::unique_ptr<IEnvironment>> environments_;
  std::vector<std::unique_ptr<IDatabase>> databases_;
  std::vector<std::unique_ptr<ISource>> sources_;
  std::vector<std::unique_ptr<IDestination>> destinations_;

  void CheckEnvironmentPort(IEnvironment* new_env);
};

} // bus


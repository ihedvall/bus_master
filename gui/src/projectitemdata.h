/*
* Copyright 2025 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <string>

#include <wx/treectrl.h>

namespace bus {
enum class ProjectItemType {
  Unknown = 0,
  Project,
  Environments,
  Environment,
  Databases,
  Database,
  Sources,
  Source,
  Destinations,
  Destination
};

class ProjectItemData : public wxTreeItemData {
 public:
  ProjectItemData() = delete;
  ProjectItemData(ProjectItemType type, std::string id);

  [[nodiscard]] ProjectItemType Type() const { return type_; }
  [[nodiscard]] const std::string& Id() const { return id_; }

 private:
  ProjectItemType type_ = ProjectItemType::Unknown;
  std::string id_;
};


} // bus



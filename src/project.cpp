/*
 * Copyright 2025 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */

#include <filesystem>
#include <memory>
#include <vector>

#include <util/logstream.h>
#include <util/ixmlfile.h>
#include <util/stringutil.h>

#include "bus/project.h"
#include "bus/ienvironment.h"

using namespace std::filesystem;
using namespace util::log;
using namespace util::xml;
using namespace util::string;

namespace bus {

bool Project::IsProjectFile(const std::string& filename) {
  try {
    path project_file(filename);
    if (!exists(project_file)) {
      throw std::runtime_error("The file doesn't exist. File: "
          + filename );
    }
    if (!is_regular_file(project_file) ) {
      throw std::runtime_error("The file is not a regular file. File: "
        + filename);
    }
    auto xml_file = CreateXmlFile();
    if (!xml_file) {
      throw std::runtime_error("The XML file was not created. File: "
        + filename);
    }
    xml_file->FileName(filename);
    if (const bool parse = xml_file->ParseFile();
        !parse) {
      throw std::runtime_error("The XML file doesn't parse. File: "
        + filename);
    }
    return true;
  } catch (const std::exception& err) {
    LOG_TRACE() << "The file is not a project file. Error: " << err.what();
  }
  return false;
}

void Project::ToProperties(std::vector<BusProperty>& properties) const {
  if (!properties.empty()) {
    properties.emplace_back();
  }
  properties.emplace_back("Project");
  properties.emplace_back("Name", Name());
  properties.emplace_back("Description", Description());
  properties.emplace_back("Config file", ConfigFile());
  properties.emplace_back();
  properties.emplace_back("Environments",std::to_string(Environments().size()));
  properties.emplace_back("Databases", std::to_string(Databases().size()));
}

bool Project::ReadConfig() {
  if (config_file_.empty()) {
    LOG_ERROR() << "Config file is not defined.";
    return false;
  }

  try {
    path fullname(config_file_);
    if (!exists(fullname)) {
      throw std::runtime_error("Config file doesn't exist. File: " +
                               config_file_);
    }
    auto xml_file = CreateXmlFile();
    if (!xml_file) {
      throw std::runtime_error("Didn't create the XML file. File: " +
                               config_file_);
    }
    xml_file->FileName(config_file_);
    const bool parse = xml_file->ParseFile();
    if (!parse) {
      throw std::runtime_error("Didn't parse the file. File: " + config_file_);
    }
    const IXmlNode* root_node = xml_file->RootNode();

    if (root_node == nullptr || !root_node->IsTagName("Project")) {
      throw std::runtime_error("The root node is not a project tag. File: " +
                               config_file_);
    }
    Name(root_node->Property<std::string>("Name"));
    Description(root_node->Property<std::string>("Description"));

    if (const IXmlNode* envs_node = root_node->GetNode("Environments");
        envs_node != nullptr) {
      IXmlNode::ChildList envs_list;
      envs_node->GetChildList(envs_list);
      for (const auto* env_node : envs_list) {
        if (env_node == nullptr || !env_node->IsTagName("Environment")) {
          continue;
        }
        auto type_name = env_node->Attribute<std::string>("type");
        if (type_name.empty()) {
          type_name = env_node->Property<std::string>("Type");
        }
        TypeOfEnvironment type = IEnvironment::TypeFromString(type_name);
        if (auto env = CreateEnvironment(type); env) {
          env->ReadConfig(*env_node);
        }
      }
    }

    if (const IXmlNode* dbs_node = root_node->GetNode("Databases");
        dbs_node != nullptr) {
      IXmlNode::ChildList db_list;
      dbs_node->GetChildList(db_list);
      for (const auto* db_node : db_list) {
        if (db_node == nullptr || !db_node->IsTagName("Database")) {
          continue;
        }
        auto type_name = db_node->Attribute<std::string>("type");
        if (type_name.empty()) {
          type_name = db_node->Property<std::string>("Type");
        }
        TypeOfDatabase type = IDatabase::TypeFromString(type_name);
        if (auto db = CreateDatabase(type); db) {
          db->ReadConfig(*db_node);
        }
      }
    }

    if (const IXmlNode* sources_node = root_node->GetNode("Sources");
        sources_node != nullptr) {
      IXmlNode::ChildList source_list;
      sources_node->GetChildList(source_list);
      for (const auto* source_node : source_list) {
        if (source_node == nullptr || !source_node->IsTagName("Source")) {
          continue;
        }
        auto type_name = source_node->Attribute<std::string>("type");
        if (type_name.empty()) {
          type_name = source_node->Property<std::string>("Type");
        }
        TypeOfSource type = ISource::TypeFromString(type_name);
        if (auto source = CreateSource(type); source) {
          source->ReadConfig(*source_node);
        }
      }
    }

    if (const IXmlNode* destinations_node = root_node->GetNode("Destinations");
        destinations_node != nullptr) {
      IXmlNode::ChildList dest_list;
      destinations_node->GetChildList(dest_list);
      for (const auto* dest_node : dest_list) {
        if (dest_node == nullptr || !dest_node->IsTagName("Destination")) {
          continue;
        }
        auto type_name = dest_node->Attribute<std::string>("type");
        if (type_name.empty()) {
          type_name = dest_node->Property<std::string>("Type");
        }
        TypeOfDestination type = IDestination::TypeFromString(type_name);
        if (auto dest = CreateDestination(type); dest) {
          dest->ReadConfig(*dest_node);
        }
      }
    }

    return true;
  } catch (const std::exception& err) {
    LOG_ERROR() << "Didn't read the config file. Error: " << err.what();
  }
  return false;
}

bool Project::WriteConfig() {
  try {
    if (config_file_.empty()) {
      throw std::runtime_error("Config file is not defined.");
    }
    path fullname(config_file_);
    if (exists(fullname)) {
      // Back up the previous file.
      BackupFiles(config_file_, true);
    } else if (fullname.has_parent_path()) {
      create_directories(fullname.parent_path());
    }
    auto xml_file = CreateXmlFile("FileWriter");
    if (!xml_file) {
      throw std::runtime_error("Didn't create the XML file. File: "
                               + config_file_);
    }
    xml_file->FileName(config_file_);
    auto& root_node = xml_file->RootName("Project");
    root_node.SetProperty("Name", Name());
    root_node.SetProperty("Description", Description());

    if (!Environments().empty()) {
      auto& envs_node = root_node.AddNode("Environments");
      for (const auto& env : Environments()) {
        if (!env) {
          continue;
        }
        env->WriteConfig(envs_node);
      }
    }

    if (!Databases().empty()) {
      auto& dbs_node = root_node.AddNode("Databases");
      for (const auto& db : Databases()) {
        if (!db) {
          continue;
        }
        db->WriteConfig(dbs_node);
      }
    }

    if (!Sources().empty()) {
      auto& sources_node = root_node.AddNode("Sources");
      for (const auto& source : Sources()) {
        if (!source) {
          continue;
        }
        source->WriteConfig(sources_node);
      }
    }

    if (!Destinations().empty()) {
      auto& destinations_node = root_node.AddNode("Destinations");
      for (const auto& dest : Destinations()) {
        if (!dest) {
          continue;
        }
        dest->WriteConfig(destinations_node);
      }
    }

    const bool write = xml_file->WriteFile();
    if (!write) {
      throw std::runtime_error("Unsuccessful save of the XML file. File: "
                               + config_file_);
    }
    return write;
  } catch (const std::exception& err) {
    LOG_ERROR() << "Didn't write the config file. Error: " << err.what();
  }
  return false;
}

IEnvironment* Project::CreateEnvironment(TypeOfEnvironment type) {
  switch (type) {
    case TypeOfEnvironment::DummyEnvironment: {
      auto dummy = std::make_unique<IEnvironment>();
      environments_.emplace_back(std::move(dummy));
      break;
    }

    default:
      return nullptr;
  }
  return environments_.empty() ? nullptr : environments_.back().get();
}

IEnvironment* Project::GetEnvironment(const std::string& name) const {
  const auto itr = std::ranges::find_if(environments_,
                                        [&name] (const auto& env) -> bool {
               return env && IEquals(env->Name(), name);
        });
  return itr != environments_.cend() ? itr->get() : nullptr;
}

/**
   * @brief Deletes an environment by its name.
   *
   * Deletes an environment by its name.
   * Note that the name string isn't a const reference.
   * The extra copy of is needed to avoid crashes.
   * If the input argument was a reference,
   * the erase deletes the environment including its name string.
   * This generates a crash in the next iteration a
   *
   * @param name Environment name.
 */
void Project::DeleteEnvironment(std::string name) {
  std::erase_if(environments_, [&name] (const auto& env) -> bool {
    return env && IEquals(env->Name(), name);
  });

}

std::vector<std::unique_ptr<IEnvironment>>& Project::Environments() {
  return environments_;
}

const std::vector<std::unique_ptr<IEnvironment>>& Project::Environments()
    const {
  return environments_;
}

IDatabase* Project::CreateDatabase(TypeOfDatabase type) {
  switch (type) {
    case TypeOfDatabase::Unknown: {
      auto dummy = std::make_unique<IDatabase>();
      databases_.emplace_back(std::move(dummy));
      break;
    }

    default:
      return nullptr;
  }
  return databases_.empty() ? nullptr : databases_.back().get();
}

IDatabase* Project::GetDatabase(const std::string& name) const {
  const auto itr = std::ranges::find_if(databases_,
                                 [&name] (const auto& db) -> bool {
                                     return db && IEquals(db->Name(), name);
                                 });
  return itr != databases_.cend() ? itr->get() : nullptr;
}

void Project::DeleteDatabase(std::string name) {
  std::erase_if(databases_, [&name] (const auto& db) -> bool {
    return db && IEquals(db->Name(), name);
  });
}
const std::vector<std::unique_ptr<IDatabase>>& Project::Databases() const {
  return databases_;
}
std::vector<std::unique_ptr<IDatabase>>& Project::Databases() {
  return databases_;
}

ISource* Project::CreateSource(TypeOfSource type) {
  switch (type) {
    case TypeOfSource::Unknown: {
      auto dummy = std::make_unique<ISource>();
      sources_.emplace_back(std::move(dummy));
      break;
    }

    default:
      return nullptr;
  }
  return sources_.empty() ? nullptr : sources_.back().get();
}

ISource* Project::GetSource(const std::string& name) const {
  const auto itr = std::ranges::find_if(sources_,
                                        [&name] (const auto& source) -> bool {
                                          return source && IEquals(source->Name(), name);
                                        });
  return itr != sources_.cend() ? itr->get() : nullptr;
}

void Project::DeleteSource(std::string name) {
  std::erase_if(sources_, [&name] (const auto& source) -> bool {
    return source && IEquals(source->Name(), name);
  });
}

const std::vector<std::unique_ptr<ISource>>& Project::Sources() const {
  return sources_;
}

std::vector<std::unique_ptr<ISource>>& Project::Sources() {
  return sources_;
}

IDestination* Project::CreateDestination(TypeOfDestination type) {
  switch (type) {
    case TypeOfDestination::Unknown: {
      auto dummy = std::make_unique<IDestination>();
      destinations_.emplace_back(std::move(dummy));
      break;
    }

    default:
      return nullptr;
  }
  return destinations_.empty() ? nullptr : destinations_.back().get();
}

IDestination* Project::GetDestination(const std::string& name) const {
  const auto itr = std::ranges::find_if(destinations_,
                                        [&name] (const auto& dest) -> bool {
                                          return dest && IEquals(dest->Name(), name);
                                        });
  return itr != destinations_.cend() ? itr->get() : nullptr;
}

void Project::DeleteDestination(std::string name) {
  std::erase_if(destinations_, [&name] (const auto& dest) -> bool {
    return dest && IEquals(dest->Name(), name);
  });
}

const std::vector<std::unique_ptr<IDestination>>& Project::Destinations() const {
  return destinations_;
}

std::vector<std::unique_ptr<IDestination>>& Project::Destinations() {
  return destinations_;
}


} // bus
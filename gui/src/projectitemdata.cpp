/*
* Copyright 2025 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */

#include "projectitemdata.h"

#include <algorithm>

namespace bus {
ProjectItemData::ProjectItemData(ProjectItemType type, std::string id)
    : type_(type),
     id_(std::move(id)) {
}

} // bus
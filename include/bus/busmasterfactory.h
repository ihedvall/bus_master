/*
 * Copyright 2025 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <memory>

#include "bus/ienvironment.h"

namespace bus {

class BusMasterFactory {
 public:
  std::unique_ptr<IEnvironment> CreateEnvironment(TypeOfEnvironment type);
};

} // bus


//
// Created by ihedv on 2025-07-05.
//

#include "../include/bus/busmasterfactory.h"

namespace bus {
std::unique_ptr<IEnvironment> BusMasterFactory::CreateEnvironment(TypeOfEnvironment type) {
  return std::unique_ptr<IEnvironment>();
}
} // bus
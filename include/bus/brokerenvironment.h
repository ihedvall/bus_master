/*
* Copyright 2025 Ingemar Hedvall
* SPDX-License-Identifier: MIT
*/

#pragma once

#include <memory>

#include "bus/ienvironment.h"
#include "bus/ibusmessagebroker.h"

namespace bus {

class BrokerEnvironment : public IEnvironment {
  public:
   BrokerEnvironment();
   ~BrokerEnvironment() override;
   void Start() override;
   void Stop() override;
  private:
   std::unique_ptr<IBusMessageBroker> broker_;
};

}  // namespace bus


/*
* Copyright 2025 Ingemar Hedvall
* SPDX-License-Identifier: MIT
 */

#include "bus/brokerenvironment.h"

#include <util/logstream.h>

#include "bus/interface/businterfacefactory.h"

using namespace util::log;

namespace bus {

BrokerEnvironment::BrokerEnvironment() {
  type_ = TypeOfEnvironment::BrokerEnvironment;
}

BrokerEnvironment::~BrokerEnvironment() {
  BrokerEnvironment::Stop();
}

void BrokerEnvironment::Start() {
  if (started_) {
    return;
  }

  operable_ = false;
  started_ = false;
  broker_.reset();

  if (SharedMemoryName().empty()) {
    LOG_ERROR() << "No shared memory name specified. Environment: "
        << Name();
    return;
  }

  auto type = BrokerType::SharedMemoryBrokerType;
  if (!HostName().empty() && Port() > 0) {
    // Enable TCP/IP on the broker.
    // Note that this might not work.
    type = BrokerType::TcpBrokerType;
  }
  broker_ = BusInterfaceFactory::CreateBroker(type);
  if (!broker_) {
    LOG_ERROR() << "Creation of the broker failed. Environment: " << Name();
    return;
  }
  broker_->Name(SharedMemoryName());
  broker_->Address(HostName());
  broker_->Port(Port());
  broker_->Start();

  operable_ = broker_->IsConnected();
  started_ = true;
  LOG_TRACE() << "Started the broker environment. Environment: " << Name();
}

void BrokerEnvironment::Stop() {
  if (broker_) {
    broker_->Stop();
    broker_.reset();
    LOG_TRACE() << "Stopped the broker environment. Environment: " << Name();
  }
  operable_ = false;
  started_ = false;
}

}  // namespace bus
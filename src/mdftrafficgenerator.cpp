/*
* Copyright 2025 Ingemar Hedvall
* SPDX-License-Identifier: MIT
 */

#include "bus/mdftrafficgenerator.h"

#include <filesystem>
#include <memory>
#include <functional>

#include <util/logstream.h>

#include <mdf/mdfreader.h>
#include <mdf/idatagroup.h>
#include <mdf/ichannelgroup.h>
#include <mdf/canbusobserver.h>
#include <bus/candataframe.h>

using namespace std::filesystem;
using namespace util::log;
using namespace mdf;

namespace bus {

MdfTrafficGenerator::MdfTrafficGenerator() {
  type_ = TypeOfSource::Mdf;
}

void MdfTrafficGenerator::Enable(bool enable) {

  enabled_ = false;
  operable_ = false;
  const bool check = CheckMdfFile();
  if (!check) {
    LOG_ERROR() << "Didn't enable the task. Name: " << Name();
    return;
  }
  const bool read = ReadMdfFile();
  if (!read) {
    LOG_ERROR() << "Didn't read the MDF file. File: " << Filename();
    return;
  }
  enabled_ = true;
  operable_ = true;

}

bool MdfTrafficGenerator::CheckMdfFile() {
  try {
    path fullname(Filename());
    if (fullname.empty()) {
      throw std::runtime_error("Empty filename.");
    }
    if (!exists(fullname)) {
      throw std::runtime_error("The file doesn't exist.");
    }
    if (!IsMdfFile(Filename())) {
      throw std::runtime_error("This is not an MDF file.");
    }
  } catch (const std::exception& err) {
    LOG_ERROR() << "Invalid MDF file. File: " << Filename()
                << ", Error: " << err.what();
    return false;
  }
  return true;
}

bool MdfTrafficGenerator::ReadMdfFile() {
  try {
    message_list_.clear();
    MdfReader reader(Filename());
    const bool read_meta_data = reader.ReadEverythingButData();
    if (!read_meta_data) {
      throw std::runtime_error(
          "Didn't find any meta-data in the file. File: " + Filename() );
    }

    const auto* mdf_file = reader.GetFile();
    if (mdf_file == nullptr) {
      throw std::runtime_error("Didn't find any MDF file. File: "
                               + Filename());
    }

    const auto* header = mdf_file->Header();
    if (header == nullptr) {
      throw std::runtime_error("Didn't find any header block. File: "
                               + Filename());
    }
    // All the samples timestamps are relative to this start time.
    start_time_ = header->StartTime();
    DataGroupList dg_list;
    mdf_file->DataGroups(dg_list);
    for (IDataGroup* data_group : dg_list) {
      if (data_group == nullptr) {
        continue;
      }
      std::vector<std::unique_ptr<CanBusObserver>> observer_list;
      for (const auto* channel_group : data_group->ChannelGroups()) {
        if (channel_group == nullptr ||
            (channel_group->Flags() & CgFlag::VlsdChannel) != 0 ||
            (channel_group->Flags() & CgFlag::BusEvent) == 0 ||
            channel_group->GetBusType() != TypeOfBus()||
            channel_group->NofSamples() == 0) {
          continue;
        }
        auto observer = std::make_unique<CanBusObserver>(*data_group,
                                                         *channel_group);
        observer->OnCanMessage = std::bind(
            &MdfTrafficGenerator::CanMessageCallback, this,
            std::placeholders::_1, std::placeholders::_2);
        observer_list.emplace_back(std::move(observer));
      }
      if (observer_list.empty()) {
        continue;
      }
      const bool data = reader.ReadData(*data_group);
      if (!data) {
        throw std::runtime_error(
            "Didn't read the CAN message data block. File: " + Filename());
      }

    }

    LOG_TRACE() << "Stored " << message_list_.size() << " CAN messages.";
    std::sort(message_list_.begin(), message_list_.end(), LessTime);
  } catch (const std::exception& err) {
    LOG_ERROR() << "Didn't read the file. Error: " << err.what()
      << ", File: " << Filename();
    return false;
  }
  return true;
}

bool MdfTrafficGenerator::CanMessageCallback(uint64_t ,
                                             const mdf::CanMessage& msg) {
  AddCanMessage(msg);
  return true;
}

bool MdfTrafficGenerator::LessTime(
    const std::unique_ptr<IBusMessage>& msg1,
    const std::unique_ptr<IBusMessage>& msg2) {
  if (!msg1 && !msg2) {
    return false;
  }
  if (!msg1 && msg2) {
    return true;
  }
  if (msg1 && !msg2) {
    return false;
  }
  return msg1->Timestamp() < msg2->Timestamp();
}

void MdfTrafficGenerator::AddCanMessage(const CanMessage& msg) {
  switch (msg.TypeOfMessage()) {
    case MessageType::CAN_DataFrame: {
      auto bus_msg = std::make_unique<CanDataFrame>();
      // The CAN message timestamp unit is seconds and might be a negative
      // value.
      int64_t rel_time = static_cast<int64_t>(msg.Timestamp() * 1'000'000'000);
      bus_msg->Timestamp(start_time_ + rel_time);
      bus_msg->BusChannel(msg.BusChannel());
      bus_msg->MessageId(msg.MessageId());
      bus_msg->CanId(msg.CanId());
      bus_msg->ExtendedId(msg.ExtendedId());
      bus_msg->Dlc(msg.Dlc());
      bus_msg->Crc(msg.Crc());
      bus_msg->DataLength(msg.DataLength());
      bus_msg->DataBytes(msg.DataBytes());
      bus_msg->Dir(msg.Dir());
      bus_msg->Srr(msg.Srr());
      bus_msg->Edl(msg.Edl());
      bus_msg->Brs(msg.Brs());
      bus_msg->Esi(msg.Esi());
      bus_msg->Rtr(msg.Rtr());
      bus_msg->WakeUp(msg.WakeUp());
      bus_msg->SingleWire(msg.SingleWire());
      bus_msg->R0(msg.R0());
      bus_msg->R1(msg.R1());
      bus_msg->FrameDuration(msg.FrameDuration());
      message_list_.emplace_back(std::move(bus_msg));
    }

    case MessageType::CAN_ErrorFrame:
    case MessageType::CAN_OverloadFrame:
    case MessageType::CAN_RemoteFrame:
    default:
      break;
  }
}

const IBusMessage* MdfTrafficGenerator::GetMessage(size_t index) const {
  return index < message_list_.size() ? message_list_[index].get() : nullptr;
}

uint64_t MdfTrafficGenerator::FirstTime() const {
  return message_list_.empty() ? 0 : message_list_[0]->Timestamp();
}

}  // namespace bus
/*
* Copyright 2025 Ingemar Hedvall
* SPDX-License-Identifier: MIT
 */

#include "bus/dbcdatabase.h"

#include <algorithm>
#include <filesystem>
#include <sstream>

#include "util/logstream.h"

using namespace std::filesystem;
using namespace util::log;
using namespace dbc;
using namespace metric;

namespace {
  void SetMetricDataType(const Signal& signal, Metric& metric) {
    const auto& enum_list = signal.EnumList();
    MetricProperty bits_prop("bits",std::to_string(signal.BitLength()) );
    metric.AddProperty(bits_prop);

    if (!enum_list.empty()) {
      std::ostringstream enum_str;
      for (const auto& [key, text] : enum_list) {
        if (!enum_str.str().empty()) {
          enum_str << ";";
        }
        std::string enum_text = text;
        std::ranges::replace(enum_text, ';', ' ');
        enum_str << key << ":" << text;
      }
      MetricProperty enum_prop("enumerate", enum_str.str());
      metric.AddProperty(enum_prop);
      metric.Type(MetricType::String);
      return;
    }

    if (signal.IsArrayValue()) {
      metric.Type(MetricType::String);
      return;
    }

    const bool no_scale = signal.Scale() == 1.0 && signal.Offset() == 0;
    if (no_scale) {
      switch (signal.DataType()) {
        case SignalDataType::SignedData:
          if (signal.BitLength() <= 8) {
            metric.Type(MetricType::Int8);
          } else if (signal.BitLength() <= 16) {
            metric.Type(MetricType::Int16);
          } else if (signal.BitLength() <= 32) {
            metric.Type(MetricType::Int32);
          } else {
            metric.Type(MetricType::Int64);
          }
          break;

        case SignalDataType::UnsignedData:
          if (signal.BitLength() <= 1) {
            metric.Type(MetricType::Boolean);
          } else if (signal.BitLength() <= 8) {
            metric.Type(MetricType::UInt8);
          } else if (signal.BitLength() <= 16) {
            metric.Type(MetricType::UInt16);
          } else if (signal.BitLength() <= 32) {
            metric.Type(MetricType::UInt32);
          } else {
            metric.Type(MetricType::UInt64);
          }
          break;

        case SignalDataType::FloatData:
          metric.Type(MetricType::Float);
          break;

        case SignalDataType::DoubleData:
          metric.Type(MetricType::Double);
          break;

        default:
          metric.Type(MetricType::Unknown);
      }
    } else {
      metric.Type(MetricType::Double);
    }
  }
}

namespace bus {
DbcDatabase::DbcDatabase() {
  type_ = TypeOfDatabase::DbcFile;
}

void DbcDatabase::Enable( bool enable) {
  IDatabase::Enable(enable);
  try {
    operable_ = false;
    enabled_ = false;
    dbc_file_.reset();
    group_list_.clear();
    metric_list_.clear();

    if (!enable) {
      return;
    }

    path filename(Filename());

    if (!exists(filename)) {
      throw std::runtime_error("The DBC file doesn't exist. File: " + Filename());
    }
    dbc_file_ = std::make_unique<DbcFile>();
    dbc_file_->Filename(Filename());
    const bool parse = dbc_file_->ParseFile();
    if (!parse) {
      std::ostringstream err;
      err << "Didn't parse the DBC file. Error: " << dbc_file_->LastError();
      throw std::runtime_error(err.str());
    }
    const auto* network = dbc_file_->GetNetwork();
    if (network == nullptr) {
      throw std::runtime_error("No network in the DBC file. File: "
                               + Filename());
    }
    for (const auto& [msg_id, msg] : network->Messages()) {
      auto* group = CreateGroup(msg.Name(), static_cast<uint32_t>(msg.Ident()));
      if (group == nullptr) {
        continue;
      }
      group->Description(msg.Comment());
      for (const auto& [signal_name, signal] : msg.Signals()) {
        auto* metric = CreateMetric(*group, signal_name);
        if (metric == nullptr) {
          continue;
        }
        metric->Description(signal.Comment());
        metric->Unit(signal.Unit());
        SetMetricDataType(signal, *metric);
        if (signal.Min() < signal.Max()) {
          MetricProperty min("min", std::to_string(signal.Min()));
          metric->AddProperty(min);
          MetricProperty max("max", std::to_string(signal.Max()));
          metric->AddProperty(max);
        }
      }
      enabled_ = true;
    }
  } catch (const std::exception& err) {
    LOG_ERROR() << "Activation error. Filename: " << Filename()
      << ", Error: " << err.what();
    operable_ = false;
    return;
  }
}


}  // namespace bus
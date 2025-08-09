/*
* Copyright 2025 Ingemar Hedvall
* SPDX-License-Identifier: MIT
 */

#pragma once

#include <memory>

#include <dbc/dbcfile.h>

#include "bus/idatabase.h"

namespace bus {

class DbcDatabase  : public IDatabase {
 public:
  DbcDatabase();
  void Enable(bool enable) override;

 private:
  std::unique_ptr<dbc::DbcFile> dbc_file_;
};

}  // namespace bus



/*
 * Copyright 2025 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */
#pragma once

#include <wx/defs.h>

namespace bus {

constexpr wxWindowID kIdLeftTree = 1;
constexpr wxWindowID kIdPropertyList = 2;
constexpr wxWindowID kIdEnvironmentList = 3;
constexpr wxWindowID kIdDatabaseList = 4;
constexpr wxWindowID kIdTimer = 5;

constexpr wxWindowID kIdLeftPanel = 20;
constexpr wxWindowID kIdPropertyPanel = 21;
constexpr wxWindowID kIdEnvironmentPanel = 22;
constexpr wxWindowID kIdDatabasePanel = 23;

constexpr wxWindowID kIdOpenLogFile = 100;
constexpr wxWindowID kIdConfigPicker = 101;
constexpr wxWindowID kIdSave = 102;
constexpr wxWindowID kIdEditProject = 103;

constexpr wxWindowID kIdAddEnvironment = 200;
constexpr wxWindowID kIdEditEnvironment = 201;
constexpr wxWindowID kIdDeleteEnvironment = 202;
constexpr wxWindowID kIdStartEnvironment = 203;
constexpr wxWindowID kIdStopEnvironment = 204;

constexpr wxWindowID kIdAddDatabase = 300;
constexpr wxWindowID kIdEditDatabase = 301;
constexpr wxWindowID kIdDeleteDatabase = 302;
constexpr wxWindowID kIdActivateDatabase = 303;
constexpr wxWindowID kIdDeactivateDatabase = 304;

constexpr wxWindowID kIdAddSource = 400;
constexpr wxWindowID kIdAddUnknownSource = 401;
constexpr wxWindowID kIdAddMdfSource = 402;

constexpr wxWindowID kIdEditSource = 410;
constexpr wxWindowID kIdDeleteSource = 411;

constexpr wxWindowID kIdAddDestination = 500;
constexpr wxWindowID kIdAddUnknownDestination = 501;
constexpr wxWindowID kIdAddMdfDestination = 502;

constexpr wxWindowID kIdEditDestination = 510;
constexpr wxWindowID kIdDeleteDestination = 511;

}
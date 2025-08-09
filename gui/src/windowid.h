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
constexpr wxWindowID kIdLogListView = 6;
constexpr wxWindowID kIdSourceList = 7;
constexpr wxWindowID kIdDestinationList = 8;

constexpr wxWindowID kIdLeftPanel = 20;
constexpr wxWindowID kIdPropertyPanel = 21;
constexpr wxWindowID kIdEnvironmentPanel = 22;
constexpr wxWindowID kIdDatabasePanel = 23;
constexpr wxWindowID kIdStartSimulation = 24;
constexpr wxWindowID kIdStopSimulation = 25;

constexpr wxWindowID kIdOpenLogFile = 100;
constexpr wxWindowID kIdConfigPicker = 101;
constexpr wxWindowID kIdSave = 102;
constexpr wxWindowID kIdEditProject = 103;
constexpr wxWindowID kIdHideLogView = 104;
constexpr wxWindowID kIdShowLogView = 105;
constexpr wxWindowID kIdAddLogMessage = 106;
constexpr wxWindowID kIdSendLogMessage = 107;

constexpr wxWindowID kIdAddEnvironment = 200;
constexpr wxWindowID kIdAddBrokerEnvironment = 201;
constexpr wxWindowID kIdEditEnvironment = 210;
constexpr wxWindowID kIdDeleteEnvironment = 211;
constexpr wxWindowID kIdEnableEnvironment = 212;
constexpr wxWindowID kIdDisableEnvironment = 213;
constexpr wxWindowID kIdStartEnvironment = 214;
constexpr wxWindowID kIdStopEnvironment = 215;

constexpr wxWindowID kIdAddDatabase = 300;
constexpr wxWindowID kIdAddDbcDatabase = 301;
constexpr wxWindowID kIdEditDatabase = 310;
constexpr wxWindowID kIdDeleteDatabase = 311;
constexpr wxWindowID kIdActivateDatabase = 312;
constexpr wxWindowID kIdDeactivateDatabase = 313;

constexpr wxWindowID kIdAddSource = 400;
constexpr wxWindowID kIdAddUnknownSource = 401;
constexpr wxWindowID kIdAddMdfSource = 402;
constexpr wxWindowID kIdEditSource = 410;
constexpr wxWindowID kIdDeleteSource = 411;
constexpr wxWindowID kIdEnableSource = 412;
constexpr wxWindowID kIdDisableSource = 413;

constexpr wxWindowID kIdAddDestination = 500;
constexpr wxWindowID kIdAddUnknownDestination = 501;
constexpr wxWindowID kIdAddMdfDestination = 502;
constexpr wxWindowID kIdEditDestination = 510;
constexpr wxWindowID kIdDeleteDestination = 511;
constexpr wxWindowID kIdEnableDestination = 512;
constexpr wxWindowID kIdDisableDestination = 513;
}
# Copyright 2021 Ingemar Hedvall
# SPDX-License-Identifier: MIT
project(BusMaster  VERSION 1.0
  DESCRIPTION "Bus Master GUI"
  LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)

add_executable(bus_master
        src/busmaster.cpp src/busmaster.h
    src/mainframe.cpp src/mainframe.h
        src/projectdocument.cpp src/projectdocument.h
        src/projectview.cpp src/projectview.h
        src/projectframe.cpp src/projectframe.h
    src/windowid.h
    src/busmaster.rc
        src/projecttree.cpp
        src/projecttree.h
        src/projectitemdata.cpp
        src/projectitemdata.h
        src/propertyview.cpp
        src/propertyview.h
        src/projectdialog.cpp
        src/projectdialog.h
        src/environmenttypedialog.cpp
        src/environmenttypedialog.h
        src/environmentdialog.cpp
        src/environmentdialog.h
        src/environmentview.cpp
        src/environmentview.h
        src/databasetypedialog.cpp
        src/databasetypedialog.h
        src/databasedialog.cpp
        src/databasedialog.h
        src/databaseview.cpp
        src/databaseview.h
        src/unknownsourcedialog.cpp
        src/unknownsourcedialog.h
        src/unknowndestinationdialog.cpp
        src/unknowndestinationdialog.h)


target_include_directories(bus_master PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_include_directories(bus_master PRIVATE ${wxWidgets_INCLUDE_DIRS})
target_include_directories(bus_master PRIVATE ${Boost_INCLUDE_DIRS})

if(MSVC)
  target_link_options(bus_master PRIVATE /SUBSYSTEM:WINDOWS)
  target_compile_definitions(bus_master PRIVATE _WIN32_WINNT=0x0A00)
elseif(MINGW)
  target_link_options(bus_master PRIVATE -mwindows -static -fstack-protector)
endif()

if(MSVC)
  target_compile_definitions(bus_master PRIVATE
          _SILENCE_ALL_CXX17_DEPRECATION_WARNINGS
          _CRT_SECURE_NO_WARNINGS
          _SILENCE_CXX20_U8PATH_DEPRECATION_WARNING)
endif()

target_link_libraries(bus_master PRIVATE util)
target_link_libraries(bus_master PRIVATE bus-master-lib)
target_link_libraries(bus_master PRIVATE sago::platform_folders)
target_link_libraries(bus_master PRIVATE Boost::locale)
target_link_libraries(bus_master PRIVATE Boost::process)
target_link_libraries(bus_master PRIVATE Boost::filesystem)
target_link_libraries(bus_master PRIVATE wxWidgets::wxWidgets)


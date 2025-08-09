# Copyright 2025 Ingemar Hedvall
# SPDX-License-Identifier: MIT

cmake_minimum_required(VERSION 3.30)
include(CMakePrintHelpers)

if("${CMAKE_TOOLCHAIN_FILE}" STREQUAL "")
    set(USE_VCPKG OFF)
else()
    set(USE_VCPKG ON)
endif()

option(BUILD_SHARED_LIBS "Static libraries are preferred" OFF)
option(MASTER_DOC "Build documentation" OFF)
option(MASTER_GUI "Building GUI application" ON)
option(MASTER_TEST "Building unit test" OFF)


if(MASTER_GUI AND USE_VCPKG)
    list(APPEND VCPKG_MANIFEST_FEATURES "gui")
endif()

project(bus-master
        VERSION 1.0
        DESCRIPTION "Bus Master Library and GUI"
        LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_DEBUG_POSTFIX d)

include(script/utillib.cmake)
include(script/busmessage.cmake)
include(script/metriclib.cmake)
include(script/dbclib.cmake)
include(script/mdflib.cmake)

if (MASTER_GUI)
    include(script/boost.cmake)
    include(script/wxwidgets.cmake)
endif()

if (MASTER_TEST)
    include(script/googletest.cmake)
endif()

if (MASTER_DOC)
    include(script/doxygen.cmake)
    include(script/mkdocs.cmake)
endif()

add_library(bus-master-lib
        src/ienvironment.cpp
        include/bus/ienvironment.h
        src/busmasterfactory.cpp
        include/bus/busmasterfactory.h
        src/busproperty.cpp
        include/bus/busproperty.h
        src/project.cpp
        include/bus/project.h
        src/idatabase.cpp
        include/bus/idatabase.h
        src/isource.cpp
        include/bus/isource.h
        src/idestination.cpp
        include/bus/idestination.h
        src/brokerenvironment.cpp
        include/bus/brokerenvironment.h
        src/dbgroup.cpp
        include/bus/dbgroup.h
        src/dbmetric.cpp
        include/bus/dbmetric.h
        src/dbcdatabase.cpp
        include/bus/dbcdatabase.h
        src/mdftrafficgenerator.cpp
        include/bus/mdftrafficgenerator.h

)

target_include_directories(bus-master-lib PUBLIC
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        ${utillib_SOURCE_DIR}/include
        ${busmessage_SOURCE_DIR}/include
        ${metriclib_SOURCE_DIR}/include
        ${dbclib_SOURCE_DIR}/include
        ${mdflib_SOURCE_DIR}/include
       )


cmake_print_properties(TARGETS bus-master-lib PROPERTIES INCLUDE_DIRECTORIES)

if (MSVC)
    # Set target to Windows 10
    target_compile_definitions(bus-master-lib PRIVATE _WIN32_WINNT=0x0A00)
endif ()

if (MASTER_GUI)
    add_subdirectory(gui)
endif ()

if (MASTER_TEST)
    enable_testing()
    add_subdirectory(test)
endif ()

if (MASTER_DOC)
    execute_process( COMMAND mkdocs build
                     WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
endif ()

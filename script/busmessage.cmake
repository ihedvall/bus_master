
# Copyright 2025 Ingemar Hedvall
# SPDX-License-Identifier: MIT

include (FetchContent)

FetchContent_Declare(bus-message-lib
        GIT_REPOSITORY https://github.com/ihedvall/bus_message_lib.git
        GIT_TAG HEAD)

set(BUS_DOC OFF)
set(BUS_TEST OFF)
set(BUS_TOOLS ON)
set(BUS_INTERFACE ON)

FetchContent_MakeAvailable(bus-message-lib)

cmake_print_variables(
        bus-message-lib_POPULATED
        bus-message-lib_SOURCE_DIR
        bus-message-lib_BINARY_DIR )
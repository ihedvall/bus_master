
# Copyright 2025 Ingemar Hedvall
# SPDX-License-Identifier: MIT

include (FetchContent)

FetchContent_Declare(utillib
        GIT_REPOSITORY https://github.com/ihedvall/utillib.git
        GIT_TAG HEAD)

set(UTIL_DOC OFF)
set(UTIL_TEST OFF)
set(UTIL_TOOLS ON)
set(UTIL_LEX OFF)

FetchContent_MakeAvailable(utillib)

cmake_print_variables(
        utillib_POPULATED
        utillib_SOURCE_DIR
        utillib_BINARY_DIR )
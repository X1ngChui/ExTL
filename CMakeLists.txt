﻿cmake_minimum_required(VERSION 3.16)
project(ExTL LANGUAGES CXX)

# ---------------------------------------------------------------------------------------
# Set C++20 as the required standard
# ---------------------------------------------------------------------------------------
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ---------------------------------------------------------------------------------------
# Set default build to release
# ---------------------------------------------------------------------------------------
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose Release or Debug" FORCE)
endif()

# ---------------------------------------------------------------------------------------
# Options
# ---------------------------------------------------------------------------------------
# Note: The EXTL_DISABLE_EXCEPTIONS option controls whether exceptions are disabled. Since ExTL
# is an Interface Library, this option is contagious to any project that links to ExTL.
option(EXTL_DISABLE_EXCEPTIONS "Disable exceptions" OFF)

# ---------------------------------------------------------------------------------------
# Create the ExTL library target
# ---------------------------------------------------------------------------------------
add_library(ExTL INTERFACE)
target_include_directories(ExTL INTERFACE ${CMAKE_SOURCE_DIR}/include)

# Disable exceptions and RTTI if EXTL_DISABLE_EXCEPTIONS is ON
# Compiler flags to disable exceptions and RTTI
if(EXTL_DISABLE_EXCEPTIONS)
    # Clang/GCC: disable exceptions
    if(CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
        target_compile_options(ExTL INTERFACE -fno-exceptions -fno-rtti)
    # MSVC: disable exceptions
    elseif(MSVC)
        target_compile_options(ExTL INTERFACE /EHs- /EHc- /GR-)
    endif()
endif()

# ---------------------------------------------------------------------------------------
# Set EXTL_MAIN_PROJECT to ON if we are building ExTL
# ---------------------------------------------------------------------------------------
# Check if spdlog is being used directly or via add_subdirectory, but allow overriding
if(NOT DEFINED EXTL_MAIN_PROJECT)
    if(CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
        set(EXTL_MAIN_PROJECT ON)
    else()
        set(EXTL_MAIN_PROJECT OFF)
    endif()
endif()

# ---------------------------------------------------------------------------------------
# If we are building the main project, configure tests
# ---------------------------------------------------------------------------------------
if(EXTL_MAIN_PROJECT)
    # Only add tests if we are the main project
    # Include the test directory and its CMakeLists.txt
    add_subdirectory(test)
endif()

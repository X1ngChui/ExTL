cmake_minimum_required(VERSION 3.16)
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
option(EXTL_BUILD_TESTs "Build tests" ON)

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

if(EXTL_BUILD_TESTs)
    # Include the test directory and its CMakeLists.txt
    add_subdirectory(test)
endif()

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
# Note: The EXTL_DISABLE_EXCEPTIONS_AND_RTTI option controls whether exceptions and RTTI are disabled.
# Since ExTL is an Interface Library, this option is contagious to any project that links to ExTL.
option(EXTL_DISABLE_EXCEPTIONS_AND_RTTI "Disable exceptions and RTTI" OFF)
# Note: The EXTL_BUILD_TESTS option controls whether the tests are built.
option(EXTL_BUILD_TESTS "Build tests" ON)
# Note: The EXTL_STRICT_CONSTRAINTS option controls whether strict noexcept constraints are enabled in ExTL.
option(EXTL_STRICT_CONSTRAINTS "Enable strict noexcept constraints in ExTL" OFF)

# ---------------------------------------------------------------------------------------
# Create the ExTL library target
# ---------------------------------------------------------------------------------------
add_library(ExTL INTERFACE)
target_include_directories(ExTL INTERFACE ${CMAKE_SOURCE_DIR}/ExTL/include)

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

if(EXTL_BUILD_TESTS)
    # Include the test directory and its CMakeLists.txt
    add_subdirectory(test)
endif()

if(EXTL_STRICT_CONSTRAINTS)
    target_compile_definitions(ExTL INTERFACE EXTL_STRICT_CONSTRAINTS=1)
else()
    target_compile_definitions(ExTL INTERFACE EXTL_STRICT_CONSTRAINTS=0)
endif()
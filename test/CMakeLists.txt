# Collect all .cpp files in the test folder
file(GLOB_RECURSE TEST_SOURCES ${CMAKE_SOURCE_DIR}/test/*.cpp)

# Add the test executable
add_executable(ExTLTest ${TEST_SOURCES})

# Link with the ExTL library
target_link_libraries(ExTLTest PRIVATE ExTL)

# Include the doctest header-only library
target_include_directories(ExTLTest PRIVATE ${CMAKE_SOURCE_DIR}/thirdparty/doctest)

# Add the test executable to CTest (if enabled)
enable_testing()
add_test(NAME ExTLTest COMMAND ExTLTest)
# This will be included only when EXTL_MAIN_PROJECT is ON and EXTL_BUILD_TEST is ON
file(GLOB_RECURSE TEST_SOURCES ${CMAKE_SOURCE_DIR}/test/*.cpp)

# Create the test executable
add_executable(ExTLTest ${TEST_SOURCES})

# Link with the ExTL library
target_link_libraries(ExTLTest PRIVATE ExTL)
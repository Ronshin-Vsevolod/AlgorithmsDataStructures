cmake_minimum_required(VERSION 3.10)
project(ascii85)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# First time trying CMake and loading tests from GitHub
option(USE_SYSTEM_GTEST "Use system version of GoogleTest instead of automatic download" ON)

add_library(ascii85_lib ascii85_lib.cpp ascii85.hpp)

add_executable(ascii85 ascii85.cpp ascii85_lib.cpp)
target_link_libraries(ascii85 ascii85_lib)

# Google Test Setup
if(USE_SYSTEM_GTEST)
    find_package(GTest REQUIRED)
    set(GTEST_LIBRARIES GTest::GTest GTest::Main)
    message(STATUS "Using system version of GoogleTest")
else()
    include(FetchContent)
    FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG release-1.12.1
    )
    FetchContent_MakeAvailable(googletest)
    set(GTEST_LIBRARIES GTest::gtest_main)
    message(STATUS "GoogleTest will be automatically downloaded from GitHub")
endif()

enable_testing()
add_executable(ascii85_test ascii85_test.cpp ascii85_lib.cpp)
target_link_libraries(ascii85_test GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(ascii85_test) 
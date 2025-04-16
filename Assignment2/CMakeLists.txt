cmake_minimum_required(VERSION 3.10)
project(Gauss_Task)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wpedantic -g")

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/external)
find_package(Eigen3 QUIET)
if(NOT EIGEN3_FOUND)
    include_directories(${CMAKE_CURRENT_SOURCE_DIR}/external/eigen)
else()
    include_directories(${EIGEN3_INCLUDE_DIRS})
endif()

# Create a header for main exception when compiling tests
file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/exclude_main.h 
"#ifdef EXCLUDE_MAIN
  #define main excluded_main_function
#endif")

add_library(Main_obj OBJECT Main.cpp)

# Object library for tests (with disabled main function)
add_library(Main_test_obj OBJECT Main.cpp)
target_compile_definitions(Main_test_obj PRIVATE EXCLUDE_MAIN=1)
set_property(TARGET Main_test_obj PROPERTY COMPILE_FLAGS 
    "-include \"${CMAKE_CURRENT_BINARY_DIR}/exclude_main.h\"")

add_executable(Main $<TARGET_OBJECTS:Main_obj>)

# Google Test
find_package(GTest REQUIRED)
enable_testing()

add_executable(Runner GoogleTest.cpp $<TARGET_OBJECTS:Main_test_obj>)
target_link_libraries(Runner GTest::GTest GTest::Main)
gtest_discover_tests(Runner)


add_custom_target(cleanall
    COMMAND ${CMAKE_COMMAND} -E echo "Cleaning build directory contents..."
    COMMAND ${CMAKE_COMMAND} -E rm -rf "*"
    COMMAND ${CMAKE_COMMAND} -E echo "Removing generated CSV files from source directory..."
    COMMAND ${CMAKE_COMMAND} -E remove ${CMAKE_CURRENT_SOURCE_DIR}/generated.csv
    COMMAND ${CMAKE_COMMAND} -E remove ${CMAKE_CURRENT_SOURCE_DIR}/solution.csv
    COMMENT "Cleaning build directory contents and generated CSV files from source directory"
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)
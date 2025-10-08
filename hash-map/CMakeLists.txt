cmake_minimum_required(VERSION 4.0)
project(cpp_hash_map)

set(CMAKE_CXX_STANDARD 20)

include(FetchContent)

FetchContent_Declare(
        gtest
        URL https://github.com/google/googletest/archive/refs/tags/v1.17.0.zip
)

FetchContent_MakeAvailable(gtest)

add_executable(test_hashmap src/tests/Test_HashMap.cpp)
target_link_libraries(test_hashmap PRIVATE GTest::gtest_main)
target_include_directories(test_hashmap PRIVATE ${CMAKE_SOURCE_DIR}/src)
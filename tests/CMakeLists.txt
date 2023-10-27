cmake_minimum_required(VERSION 3.20)
project(R3_TESTS VERSION 1.0.0 LANGUAGES CXX C)

add_executable(TEST_CUBE test_cube.cpp)
target_link_libraries(TEST_CUBE PUBLIC R3_ENGINE)

cmake_minimum_required(VERSION 3.20)
project(R3 VERSION 1.0.0 LANGUAGES CXX C)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

file(COPY shaders DESTINATION "${CMAKE_BINARY_DIR}/engine")
file(COPY textures DESTINATION "${CMAKE_BINARY_DIR}/engine")

find_package(OpenGL REQUIRED)

add_subdirectory(engine)

# add_subdirectory(editor)
# target_link_libraries(R3_EDITOR PUBLIC R3_ENGINE)

add_executable(test_cube tests/test_cube.cpp)
target_link_libraries(test_cube PUBLIC R3_ENGINE)


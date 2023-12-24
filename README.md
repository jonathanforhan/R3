# R3 Game Engine

## Build

### Dev Dependencies

- C++20 Compiler
- CMake (version 3.20+)
- Python3

### Dev Dependencies (optional)

- wsl
- clang-format - not optional for contributions
- Doxygen
- npm - for stylized Doxygen docs
- VulkanSDK - for Vulkan Builds
- Optick-GUI  - GUI front-end to profiler (https://github.com/bombomby/optick/releases)

### Build steps

Windows and Unix

```git clone --recurse-submodules https://github.com/jonathanforhan/R3.git```

```mkdir build ; cmake -S . -B ./build --preset $(PRESET*) ; cmake --build ./build```

\* see table

#### CMake Presets

| Debug                | Release                | Distribution        |
|:-------------------- |:---------------------- |:------------------- |
| MSVC-Vulkan-Debug	   | MSVC-Vulkan-Release    | MSVC-Vulkan-Dist    |
| MSVC-OpenGL-Debug    | MSVC-OpenGL-Release    | MSVC-OpenGL-Dist    |
| Clang-Vulkan-Debug   | Clang-Vulkan-Release   | Clang-Vulkan-Dist	  |
| Clang-OpenGL-Debug   | Clang-OpenGL-Release   | Clang-OpenGL-Dist   |
| GCC-Vulkan-Debug	   | GCC-Vulkan-Release     | GCC-Vulkan-Dist     |
| GCC-OpenGL-Debug     | GCC-OpenGL-Release     | GCC-OpenGL-Dist     |

All builds are 64-bit

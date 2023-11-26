# R3 Game Engine

## Build

### Dev Dependencies

- CMake (version 3.20+)

### Dev Dependencies (optional)

- wsl (if windows)
- clang-format (not optional for contributions)
- Doxygen

### Build steps

Windows and Unix

```git clone --recurse-submodules https://github.com/jonathanforhan/R3.git```

```mkdir build ; cmake -S . -B ./build --preset $(PRESET *see table*); cmake --build ./build```

**NOTE** must use developer Powershell.exe for Windows build, Windows builds use cl.exe for VS compatibility, Linux-* will also work if you are using Cygwin or MinGW

#### CMake Presets

| Debug                | Release                | Distribution        |
|:-------------------- |:---------------------- |:------------------- |
| Windows-Vulkan-Debug | Windows-Vulkan-Release | Windows-Vulkan-Dist |
| Windows-OpenGL-Debug | Windows-OpenGL-Release | Windows-OpenGL-Dist |
| Linux-Vulkan-Debug   | Linux-Vulkan-Release   | Linux-Vulkan-Dist	  |
| Linux-OpenGL-Debug   | Linux-OpenGL-Release   | Linux-OpenGL-Dist   |

All builds are 64-bit

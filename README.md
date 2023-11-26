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
```mkdir build ; cd build ; cmake -S . -B ./build --preset $(PRESET); cmake --build ./build```

**NOTE** must use developer Powershell.exe for Windows build, Windows builds use cl.exe for VS compatibility, Linux64 will also work if you are using Cygwin or MinGW

| CMake Presets
|:---
| Win64-Vulkan-Debug
| Win64-Vulkan-Release
| Win64-Vulkan-Dist
| Win64-OpenGL-Debug
| Win64-OpenGL-Release
| Win64-OpenGL-Dist
| Win32-Vulkan-Debug
| Win32-Vulkan-Release
| Win32-Vulkan-Dist
| Win32-OpenGL-Debug
| Win32-OpenGL-Release
| Win32-OpenGL-Dist
| Linux64-Vulkan-Debug
| Linux64-Vulkan-Release
| Linux64-Vulkan-Dist
| Linux64-OpenGL-Debug
| Linux64-OpenGL-Release
| Linux64-OpenGL-Dist
| Linux32-Vulkan-Debug
| Linux32-Vulkan-Release
| Linux32-Vulkan-Dist
| Linux32-OpenGL-Debug
| Linux32-OpenGL-Release
| Linux32-OpenGL-Dist

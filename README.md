# R3 Game Engine

## Renderer

R3 currently uses OpenGL, after the SNHU software hack-a-thon I plan to change over to Vulkan and DX12

## Build

### Dev Dependencies

- CMake (version 3.20+)
- Qt (version 6)

### Dev Dependencies (optional)

- wsl (if windows)
- clang-format (not optional for contributions)

### Build steps

Windows and Unix

```mkdir build ; cd build ; cmake .. ; cmake --build .```

CMake will try find your QT install, however if using Cygwin you may need to specify -DCMAKE_PREFIX_PATH='C:/Qt/6.x.x/mingw_64/'

#pragma once
#include <cstdint>
#include "hal/opengl/Window.hpp"

namespace R3 {

#if R3_OPENGL
using Window = R3::opengl::Window;
#endif

} // namespace R3

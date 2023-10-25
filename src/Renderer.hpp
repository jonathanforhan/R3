#pragma once
#include "hal/opengl/Renderer.hpp"

namespace R3 {

#if R3_OPENGL
using Renderer = R3::opengl::Renderer;
#endif

} // namespace R3

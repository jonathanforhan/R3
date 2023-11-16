#pragma once

namespace R3 {

// Engine Version
constexpr auto ENGINE_VERSION_MAJOR = 0;
constexpr auto ENGINE_VERSION_MINOR = 0;
constexpr auto ENGINE_VERSION_PATCH = 0;

#if R3_OPENGL

// OpenGL Version
constexpr auto OPENGL_VERSION_MAJOR = 4;
constexpr auto OPENGL_VERSION_MINOR = 6;

#endif // R3_OPENGL

// glTF Version
constexpr auto GLTF_VERSION_MAJOR = 2;
constexpr auto GLTF_VERSION_MINOR = 0;

// glb Container Version
constexpr auto GLB_VERSION = 2;

} // namespace R3
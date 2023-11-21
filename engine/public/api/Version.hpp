#pragma once

namespace R3 {

constexpr auto ENGINE_VERSION_MAJOR = 0; ///< @brief Engine Version Major
constexpr auto ENGINE_VERSION_MINOR = 0; ///< @brief Engine Version Minor
constexpr auto ENGINE_VERSION_PATCH = 0; ///< @brief Engine Version Patch

#if R3_OPENGL

constexpr auto OPENGL_VERSION_MAJOR = 4; ///< @brief OpenGL Version Major
constexpr auto OPENGL_VERSION_MINOR = 6; ///< @brief OpenGL Version Minor

#endif // R3_OPENGL

constexpr auto GLTF_VERSION_MAJOR = 2; ///< @brief glTF Version Major
constexpr auto GLTF_VERSION_MINOR = 0; ///< @brief glTF Version Minor

constexpr auto GLB_VERSION = 2; ///< @brief GLB Container Version

} // namespace R3
#pragma once

/// @brief Provides wrapper around navtive OS window surface

#include "render/RenderApi.hpp"

namespace R3 {

/// @brief Surface Specification
struct R3_API SurfaceSpecification {
    const Instance& instance; ///< Instance
    const Window& window;     ///< Window
};

/// @brief Abstract over platform-specific Surface models of how to draw to the screen
class R3_API Surface : public NativeRenderObject {
public:
    DEFAULT_CONSTRUCT(Surface);
    NO_COPY(Surface);
    DEFAULT_MOVE(Surface);

    /// @brief Construct Surface from spec
    /// @param spec
    Surface(const SurfaceSpecification& spec);

    /// @brief Destroy Surface
    ~Surface();

private:
    Ref<const Instance> m_instance;
};

} // namespace R3
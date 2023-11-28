#pragma once
#include "render/NativeRenderObject.hpp"
#include "render/RenderFwd.hpp"

namespace R3 {

/// @brief Surface Specification
struct SurfaceSpecification {
    const Instance* instance; ///< @brief Valid non-null Instance
    const Window* window;     ///< @brief Valid non-null Window
};

/// @brief Abstract over platform-specific Surface models of how to draw to the screen
class Surface : public NativeRenderObject {
public:
    /// @brief Create Surface using Window extensions
    /// @param spec 
    void create(const SurfaceSpecification& spec);

    /// @brief Free Surface
    void destroy();

private:
    SurfaceSpecification m_spec;
};

} // namespace R3
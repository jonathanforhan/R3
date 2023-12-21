#pragma once

#include "api/Ref.hpp"
#include "render/NativeRenderObject.hpp"
#include "render/RenderFwd.hpp"

namespace R3 {

/// @brief Surface Specification
struct SurfaceSpecification {
    Ref<const Instance> instance; ///< @brief Valid non-null Instance
    Ref<const Window> window;     ///< @brief Valid non-null Window
};

/// @brief Abstract over platform-specific Surface models of how to draw to the screen
class Surface : public NativeRenderObject {
public:
    Surface() = default;
    Surface(const SurfaceSpecification& spec);
    Surface(Surface&&) noexcept = default;
    Surface& operator=(Surface&&) noexcept = default;
    ~Surface();

private:
    Ref<const Instance> m_instance;
};

} // namespace R3
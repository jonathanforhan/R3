#pragma once
#include "render/NativeRenderObject.hpp"
#include "render/RenderFwd.hpp"

namespace R3 {

struct SurfaceSpecification {
    const Instance* instance;
    const Window* window;
};

class Surface : public NativeRenderObject {
public:
    void create(const SurfaceSpecification& spec);
    void destroy();

private:
    SurfaceSpecification m_spec;
};

} // namespace R3
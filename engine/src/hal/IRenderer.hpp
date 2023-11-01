#pragma once
#include "api/Types.hpp"

namespace R3 {

enum class RendererPrimitive {
    Triangles,
    Lines,
};

enum class CullMode {
    Back,
    Front,
    None,
};

class IRenderer {
protected:
    IRenderer() = default;

public:
    virtual void predraw() = 0;
    virtual void draw(RendererPrimitive primitive, uint32 n_indices) = 0;
};

} // namespace R3

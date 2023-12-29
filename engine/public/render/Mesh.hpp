#pragma once

#include <R3>
#include "render/Material.hpp"

namespace R3 {

struct R3_API Mesh {
    DEFAULT_CONSTRUCT(Mesh);
    NO_COPY(Mesh);
    DEFAULT_MOVE(Mesh);

    void destroy() {}

    id vertexBuffer;
    id indexBuffer;
    id pipeline;
    Material material;
};

} // namespace R3

#pragma once

#include <R3>
#include "render/Material.hpp"

namespace R3 {

class R3_API Scene;

struct R3_API Mesh {
    DEFAULT_CONSTRUCT(Mesh);
    NO_COPY(Mesh);
    DEFAULT_MOVE(Mesh);

    void destroy(Scene* parentScene);

    id vertexBuffer = ~id(0);
    id indexBuffer = ~id(0);
    id pipeline = ~id(0);
    Material material;
};

} // namespace R3

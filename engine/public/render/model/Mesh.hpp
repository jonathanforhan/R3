#pragma once

#include <R3>
#include "render/model/Material.hpp"

namespace R3 {

class R3_API Scene;

struct R3_API Mesh {
    void destroy(Scene* parentScene);

    id vertexBuffer = undefined;
    id indexBuffer = undefined;
    id pipeline = undefined;
    Material material;
};

} // namespace R3

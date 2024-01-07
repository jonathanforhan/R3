#pragma once

#include <R3>
#include "render/GraphicsPipeline.hpp"
#include "render/IndexBuffer.hpp"
#include "render/VertexBuffer.hpp"
#include "render/model/Material.hpp"

namespace R3 {

class R3_API Scene;

struct R3_API Mesh {
    VertexBuffer vertexBuffer;
    IndexBuffer<uint32> indexBuffer;
    GraphicsPipeline pipeline;
    Material material;
};

} // namespace R3

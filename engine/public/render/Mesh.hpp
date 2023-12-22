#pragma once

#include "render/GraphicsPipeline.hpp"
#include "render/IndexBuffer.hpp"
#include "render/Material.hpp"
#include "render/VertexBuffer.hpp"

namespace R3 {

struct Mesh {
    void destroy();

    // Mesh has handles to index it's resources
    VertexBuffer::ID vertexBuffer;
    IndexBuffer<uint32>::ID indexBuffer;
    GraphicsPipeline::ID pipeline;
    Material material;
};

} // namespace R3

#include "render/Mesh.hpp"

#include "render/ResourceManager.hxx"

namespace R3 {

void Mesh::destroy() {
    GlobalResourceManager().freeVertexBuffer(vertexBuffer);
    GlobalResourceManager().freeIndexBuffer(indexBuffer);
    GlobalResourceManager().freeGraphicsPipeline(pipeline);

    material.destroy();
}

} // namespace R3
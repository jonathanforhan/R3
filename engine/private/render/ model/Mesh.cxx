#include "render/model/Mesh.hpp"

#include "core/Engine.hpp"
#include "core/Scene.hpp"
#include "render/ResourceManager.hxx"

namespace R3 {

void Mesh::destroy(Scene* parentScene) {
    auto* resourceManager = reinterpret_cast<ResourceManager*>(parentScene->resourceManager);
    resourceManager->freeVertexBuffer(vertexBuffer);
    resourceManager->freeIndexBuffer(indexBuffer);
    resourceManager->freeGraphicsPipeline(pipeline);
    material.destroy(parentScene);
}

} // namespace R3

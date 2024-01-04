#include "render/model/Material.hpp"

#include "render/ResourceManager.hxx"

namespace R3 {

void Material::destroy(Scene* parentScene) {
    auto* resourceManager = reinterpret_cast<ResourceManager*>(parentScene->resourceManager);

    resourceManager->freeDescriptorPool(descriptorPool);
    resourceManager->freeTexture(textures.albedo);
    resourceManager->freeTexture(textures.metallicRoughness);
    resourceManager->freeTexture(textures.normal);
    resourceManager->freeTexture(textures.ambientOcclusion);
    resourceManager->freeTexture(textures.emissive);

    for (auto& uniform : uniforms) {
        resourceManager->freeUniform(uniform);
    }
}

} // namespace R3
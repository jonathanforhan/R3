#include "render/Material.hpp"

#include "render/ResourceManager.hxx"

namespace R3 {

void Material::destroy() {
    GlobalResourceManager().freeDescriptorPool(descriptorPool);

    GlobalResourceManager().freeTexture(textures.albedo);
    GlobalResourceManager().freeTexture(textures.metallicRoughness);
    GlobalResourceManager().freeTexture(textures.normal);
    GlobalResourceManager().freeTexture(textures.ambientOcclusion);
    GlobalResourceManager().freeTexture(textures.emissive);

    for (UniformBuffer::ID uniform : uniforms) {
        GlobalResourceManager().freeUniform(uniform);
    }
}

} // namespace R3
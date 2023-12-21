#include "render/Material.hpp"

#include "core/Engine.hpp"
#include "render/vulkan/vulkan-UniformBufferObject.hxx"

namespace R3 {

Material::Material(const MaterialSpecification& spec) {
    m_descriptorPool = DescriptorPool({
        .logicalDevice = Engine::renderer().logicalDevice(),
        .descriptorSetCount = MAX_FRAMES_IN_FLIGHT,
    });

    std::vector<TextureDescriptor> textures;
    textures.reserve(5);

    if (spec.albedo.has_value())
        textures.push_back({*spec.albedo.value()});

    if (spec.metallicRoughness.has_value())
        textures.push_back({*spec.metallicRoughness.value()});

    if (spec.normal.has_value())
        textures.push_back({*spec.normal.value()});

    if (spec.ambientOcculsion.has_value())
        textures.push_back({*spec.ambientOcculsion.value()});

    if (spec.emissive.has_value())
        textures.push_back({*spec.emissive.value()});

    m_uniforms.resize(MAX_FRAMES_IN_FLIGHT);
    for (auto& uniformBuffer : m_uniforms) {
        uniformBuffer = UniformBuffer({
            .physicalDevice = Engine::renderer().physicalDevice(),
            .logicalDevice = Engine::renderer().logicalDevice(),
            .bufferSize = sizeof(vulkan::UniformBufferObject),
        });
    }

    std::vector<UniformDescriptor> uniformDescriptors;
    for (uint32 i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        uniformDescriptors.push_back({m_uniforms[i], 0});
    };

    auto& descriptorSets = m_descriptorPool.descriptorSets();
    for (uint32 i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        descriptorSets[i].bindResources({uniformDescriptors, textures});
    }
};

} // namespace R3
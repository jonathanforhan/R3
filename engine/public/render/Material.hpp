#pragma once

#include <optional>
#include "render/DescriptorPool.hpp"
#include "render/UniformBuffer.hpp"

namespace R3 {

struct MaterialSpecification {
    std::optional<Ref<const TextureBuffer>> albedo;
    std::optional<Ref<const TextureBuffer>> metallicRoughness;
    std::optional<Ref<const TextureBuffer>> normal;
    std::optional<Ref<const TextureBuffer>> ambientOcculsion;
    std::optional<Ref<const TextureBuffer>> emissive;
};

class Material {
public:
    Material() = default;
    Material(const MaterialSpecification& spec);
    Material(const Material&) = delete;
    Material(Material&&) noexcept = default;
    Material& operator=(Material&&) noexcept = default;

    DescriptorPool& descriptorPool() { return m_descriptorPool; }
    DescriptorSet& descriptorAt(usize i) { return m_descriptorPool.descriptorSets()[i]; }
    std::vector<UniformBuffer>& uniforms() { return m_uniforms; }

private:
    DescriptorPool m_descriptorPool;
    std::vector<UniformBuffer> m_uniforms;
};

} // namespace R3
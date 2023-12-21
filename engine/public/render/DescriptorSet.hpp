#pragma once

#include <span>
#include <vector>
#include "api/Types.hpp"
#include "render/NativeRenderObject.hpp"
#include "render/RenderFwd.hpp"

namespace R3 {

struct DescriptorSetSpecification {
    Ref<const LogicalDevice> logicalDevice;
    Ref<const DescriptorPool> descriptorPool;
    Ref<const DescriptorSetLayout> descriptorSetLayout;
    uint32 descriptorSetCount;
};

struct UniformDescriptor {
    const UniformBuffer& uniform;
    uint32 binding;
    usize offset;
    usize range;
};

struct TextureDescriptor {
    const TextureBuffer& texture;
};

struct DescriptorSetBindingSpecification {
    std::span<const UniformDescriptor> uniformDescriptors;
    std::span<const TextureDescriptor> textureDescriptors;
};

class DescriptorSet : public NativeRenderObject {
public:
    DescriptorSet() = default;

    static std::vector<DescriptorSet> allocate(const DescriptorSetSpecification& spec);

    void bindResources(const DescriptorSetBindingSpecification& spec);

private:
    Ref<const LogicalDevice> m_logicalDevice;
};

} // namespace R3
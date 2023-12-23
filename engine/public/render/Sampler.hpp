#pragma once

#include "render/NativeRenderObject.hpp"
#include "render/RenderFwd.hpp"

namespace R3 {

struct SamplerSpecification {
    const PhysicalDevice& physicalDevice;
    const LogicalDevice& logicalDevice;
    uint32 mipLevels;
};

class Sampler : public NativeRenderObject {
public:
    Sampler() = default;
    Sampler(const SamplerSpecification& spec);
    Sampler(Sampler&&) noexcept = default;
    Sampler& operator=(Sampler&&) noexcept = default;
    ~Sampler();

private:
    Ref<const LogicalDevice> m_logicalDevice;
};

} // namespace R3

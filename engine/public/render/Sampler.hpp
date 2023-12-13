#pragma once

#include "render/NativeRenderObject.hpp"
#include "render/RenderFwd.hpp"

namespace R3 {

struct SamplerSpecification {
    Ref<const PhysicalDevice> physicalDevice;
    Ref<const LogicalDevice> logicalDevice;
};

class Sampler : public NativeRenderObject {
public:
    Sampler() = default;
    Sampler(const SamplerSpecification& spec);
    Sampler(Sampler&&) noexcept = default;
    Sampler& operator=(Sampler&&) noexcept = default;
    ~Sampler();

private:
    SamplerSpecification m_spec;
};

} // namespace R3
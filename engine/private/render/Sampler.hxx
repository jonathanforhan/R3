#pragma once

/// @file Sampler.hxx

#include "render/RenderApi.hxx"

namespace R3 {

/// @brief Sampler Specification
struct SamplerSpecification {
    const PhysicalDevice& physicalDevice; ///< PhysicalDevice
    const LogicalDevice& logicalDevice;   ///< LogicalDevice
    uint32 mipLevels;                     ///< Mip levels
};

/// @brief Sampler used to read image data and apply various filtering like mipmapping
class Sampler : public NativeRenderObject {
public:
    DEFAULT_CONSTRUCT(Sampler);
    NO_COPY(Sampler);
    DEFAULT_MOVE(Sampler);

    /// @brief Construct Sampler from spec
    /// @param spec
    Sampler(const SamplerSpecification& spec);

    /// @brief Destroy Sampler
    ~Sampler();

private:
    Ref<const LogicalDevice> m_logicalDevice;
};

} // namespace R3
#pragma once

/// @file DescriptorPool.hpp

#include <vector>
#include "render/DescriptorSet.hpp"
#include "render/DescriptorSetLayout.hpp"
#include "render/RenderApi.hpp"

namespace R3 {

/// @brief Descriptor Pool Specification
struct DescriptorPoolSpecification {
    const LogicalDevice& logicalDevice; ///< LogicalDevice
    uint32 descriptorSetCount;          ///< Number of DescriptorSets to allocate
};

/// @brief DescriptorPool owns and manages DescriptorSets
/// DescriptorPools are allocated by GlobalResourceManager
class DescriptorPool : public NativeRenderObject {
public:
    DEFAULT_CONSTRUCT(DescriptorPool);
    NO_COPY(DescriptorPool);
    DEFAULT_MOVE(DescriptorPool);

    /// @brief Construct DescriptorPool from spec
    /// @param spec
    DescriptorPool(const DescriptorPoolSpecification& spec);

    /// @brief Free pool and DescriptorSets
    ~DescriptorPool();

    /// @brief Query DesciptorSetLayout used by this pool
    /// @return Layout
    const DescriptorSetLayout& layout() const { return m_layout; }

    /// @brief Query DescriptorSets owned by this pool
    /// @return sets
    std::vector<DescriptorSet>& descriptorSets() { return m_descriptorSets; }

private:
    Ref<const LogicalDevice> m_logicalDevice;
    DescriptorSetLayout m_layout;
    std::vector<DescriptorSet> m_descriptorSets;
};

} // namespace R3

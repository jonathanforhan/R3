#pragma once

/// @file PhysicalDevice.hpp
/// Abstraction over Graphics Hardware

#include <span>
#include <vector>
#include "render/RenderApi.hpp"

namespace R3 {

/// @brief Physical Device Specification
struct PhysicalDeviceSpecification {
    const Instance& instance;                   ///< Instance
    const Surface& surface;                     ///< Surface
    const std::vector<const char*>& extensions; ///< GPU extensions
};

/// @brief Abstraction over API specific representation of Hardware/GPU
class PhysicalDevice : public NativeRenderObject {
public:
    DEFAULT_CONSTRUCT(PhysicalDevice);
    NO_COPY(PhysicalDevice);
    DEFAULT_MOVE(PhysicalDevice);

    /// @brief Obtain a Physical Device from spec
    /// @param spec
    PhysicalDevice(const PhysicalDeviceSpecification& spec);

    /// @brief Query the PhysicalDevice memory properties to find memory type
    /// this is a Vulkanism and may well be noops in other APIs
    /// @param typeFilter bitmask used to filter valid memory types
    /// @param propertyFlags flags that indicate desired memory type
    /// @return memory type index
    uint32 queryMemoryType(uint32 typeFilter, uint64 propertyFlags) const;

    /// @brief Query GPU Extensions
    /// @return extensions
    std::span<const char* const> extensions() const { return m_extensions; }

    /// @brief Query Sample count
    /// @return Number of Samples
    uint8 sampleCount() const { return m_sampleCount; }

private:
    // ranks GPU based on several factor to determine best fit
    int32 evaluateDevice(const NativeRenderObject& deviceHandle) const;

    // Ensures all required extensions from PhysicalDeviceSpecification are present on specified device
    bool checkExtensionSupport(const NativeRenderObject& deviceHandle) const;

private:
    Ref<const Surface> m_surface;
    std::vector<const char*> m_extensions;
    uint8 m_sampleCount;
};

} // namespace R3

#pragma once

/// Abstraction over Graphics Hardware

#include "render/RenderApi.hpp"

namespace R3 {

/// @brief Physical Device Specification
struct R3_API PhysicalDeviceSpecification {
    const Instance& instance;                   ///< Instance
    const Surface& surface;                     ///< Surface
    const std::vector<const char*>& extensions; ///< GPU extensions
};

/// @brief Abstraction over API specific representation of Hardware/GPU
class R3_API PhysicalDevice : public NativeRenderObject {
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
    [[nodiscard]] uint32 queryMemoryType(uint32 typeFilter, uint64 propertyFlags) const;

    /// @brief Query GPU Extensions
    /// @return extensions
    [[nodiscard]] constexpr std::span<const char* const> extensions() const { return m_extensions; }

    /// @brief Query Sample count
    /// @return Number of Samples
    [[nodiscard]] constexpr uint8 sampleCount() const { return m_sampleCount; }

private:
    // ranks GPU based on several factor to determine best fit
    [[nodiscard]] int32 evaluateDevice(const NativeRenderObject& deviceHandle) const;

    // Ensures all required extensions from PhysicalDeviceSpecification are present on specified device
    [[nodiscard]] bool checkExtensionSupport(const NativeRenderObject& deviceHandle) const;

private:
    Ref<const Surface> m_surface;
    std::vector<const char*> m_extensions;
    uint8 m_sampleCount = undefined;
};

} // namespace R3

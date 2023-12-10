#pragma once

#include <span>
#include <vector>
#include "api/Types.hpp"
#include "render/NativeRenderObject.hpp"
#include "render/RenderFwd.hpp"

namespace R3 {

/// @brief Physical Device Specification
struct PhysicalDeviceSpecification {
    Ref<const Instance> instance;        ///< @brief Valid non-null Instance
    Ref<const Surface> surface;          ///< @brief Valid non-null Surface
    std::vector<const char*> extensions; ///< @brief GPU extensions
};

/// @brief Abstraction over API specific representation of Hardware/GPU
class PhysicalDevice : public NativeRenderObject {
public:
    PhysicalDevice() = default;
    PhysicalDevice(const PhysicalDeviceSpecification& spec);
    PhysicalDevice(PhysicalDevice&&) noexcept = default;
    PhysicalDevice& operator=(PhysicalDevice&&) noexcept = default;

    /// @brief Query the PhysicalDevice memory properties to find memory type
    /// this is a Vulkanism and may well be noops in other APIs
    /// @param typeFilter bitmask used to filter valid memory types
    /// @param propertyFlags flags that indicate desired memory type
    /// @return memory type index
    uint32 queryMemoryType(uint32 typeFilter, uint64 propertyFlags) const;

    /// @brief Query GPU Extensions
    /// @return extensions
    std::span<const char* const> extensions() const { return m_spec.extensions; }

private:
    // ranks GPU based on several factor to determine best fit
    int32 evaluateDevice(Handle deviceHandle) const;

    // Ensures all required extensions from PhysicalDeviceSpecification are present on specified device
    bool checkExtensionSupport(Handle deviceHandle) const;

private:
    PhysicalDeviceSpecification m_spec;
};

} // namespace R3

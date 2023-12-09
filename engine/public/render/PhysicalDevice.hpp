#pragma once

#include <vector>
#include "render/NativeRenderObject.hpp"
#include "render/RenderFwd.hpp"

namespace R3 {

/// @brief Physical Device Specification
struct PhysicalDeviceSpecification {
    const Instance* instance;            ///< @brief Valid non-null Instance
    const Surface* surface;              ///< @brief Valid non-null Surface
    std::vector<const char*> extensions; ///< @brief GPU extensions
};

/// @brief Abstraction over API specific representation of Hardware/GPU
class PhysicalDevice : public NativeRenderObject {
public:
    /// @brief Select the optimal physical device to use for rendering
    /// @param spec
    void select(const PhysicalDeviceSpecification& spec);


    /// @brief Query the PhysicalDevice memory properties to find memory type
    /// this is a Vulkanism and may well be noops in other APIs
    /// @param typeFilter bitmask used to filter valid memory types
    /// @param propertyFlags flags that indicate desired memory type
    /// @return memory type index
    uint32 queryMemoryType(uint32 typeFilter, uint64 propertyFlags) const;

    /// @brief Query GPU Extensions
    /// @return extensions
    const std::vector<const char*>& extensions() const { return m_spec.extensions; }

private:
    // ranks GPU based on several factor to determine best fit
    int32 evaluateDevice(Handle deviceHandle) const;

    // Ensures all required extensions from PhysicalDeviceSpecification are present on specified device
    bool checkExtensionSupport(Handle deviceHandle) const;

private:
    PhysicalDeviceSpecification m_spec;
};

} // namespace R3

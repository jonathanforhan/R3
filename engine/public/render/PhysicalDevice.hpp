#pragma once
#include <vector>
#include "api/Types.hpp"
#include "render/NativeRenderObject.hpp"
#include "render/RenderFwd.hpp"

namespace R3 {

struct PhysicalDeviceSpecification {
    const Instance* instance;
    const Surface* surface;
    std::vector<const char*> extensions;
};

class PhysicalDevice : public NativeRenderObject {
public:
    void select(const PhysicalDeviceSpecification& spec);

    uint32 queryMemoryType(uint32 typeFilter, uint64 propertyFlags) const;

    const std::vector<const char*>& extensions() const { return m_spec.extensions; }

private:
    int32 evaluateDevice(Handle deviceHandle) const;

    /// @brief Ensures all required extensions from PhysicalDeviceSpecification are present on specified device
    bool checkExtensionSupport(Handle deviceHandle) const;

private:
    PhysicalDeviceSpecification m_spec;
};

} // namespace R3

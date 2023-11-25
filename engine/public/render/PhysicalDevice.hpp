#pragma once
#include "api/Types.hpp"
#include "render/Instance.hpp"
#include "render/NativeRenderObject.hpp"

namespace R3 {

struct PhysicalDeviceSpecification {
    Instance* instance;
};

class PhysicalDevice : public NativeRenderObject {
public:
    void select(const PhysicalDeviceSpecification& spec);

private:
    int32 evaluateDevice(Handle deviceHandle) const;

private:
    PhysicalDeviceSpecification m_spec;
};

} // namespace R3

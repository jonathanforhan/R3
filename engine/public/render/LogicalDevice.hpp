#pragma once
#include "render/Instance.hpp"
#include "render/NativeRenderObject.hpp"
#include "render/PhysicalDevice.hpp"

namespace R3 {

struct LogicalDeviceSpecification : public NativeRenderObject {
    Instance* instance;
    PhysicalDevice* physicalDevice;
};

class LogicalDevice {
public:
    void create(const LogicalDeviceSpecification& spec);
    void destroy();

private:
    LogicalDeviceSpecification m_spec;
};

} // namespace R3
#if R3_VULKAN

#include "render/LogicalDevice.hpp"

#include <vulkan/vulkan.h>
#include "render/Queues.hpp"
#include "api/Check.hpp"
#include "api/Ensure.hpp"
#include "api/Log.hpp"

namespace R3 {

void LogicalDevice::create(const LogicalDeviceSpecification& spec) {
    CHECK(spec.instance != nullptr);
    CHECK(spec.physicalDevice != nullptr);
    m_spec = spec;

    QueueFamilyIndices queueFamilyIndices = Queues::queryQueueFamilyIndices(m_spec.physicalDevice->handle());
}

void LogicalDevice::destroy() {
}

} // namespace R3

#endif // R3_VULKAN

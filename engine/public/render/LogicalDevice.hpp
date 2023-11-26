#pragma once
#include "render/NativeRenderObject.hpp"
#include "render/Queue.hpp"
#include "render/RenderFwd.hpp"

namespace R3 {

struct LogicalDeviceSpecification {
    const Instance* instance;
    const Surface* surface;
    const PhysicalDevice* physicalDevice;
};

class LogicalDevice : public NativeRenderObject {
public:
    void create(const LogicalDeviceSpecification& spec);
    void destroy();

    const Queue& graphicsQueue() const { return m_graphicsQueue; }
    const Queue& presentattionQueue() const { return m_presentationQueue; }

private:
    LogicalDeviceSpecification m_spec;
    Queue m_graphicsQueue;
    Queue m_presentationQueue;
};

} // namespace R3
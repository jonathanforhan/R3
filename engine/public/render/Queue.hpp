#pragma once
#include "api/Types.hpp"
#include "render/NativeRenderObject.hpp"

namespace R3 {

class LogicalDevice;

enum class QueueType {
    Graphics,
    Presentation,
    Compute,
};

struct QueueFamilyIndices {
private:
    QueueFamilyIndices() = default;

public:
    int32 graphics = -1;
    int32 presentation = -1;
    bool isValid() const { return graphics >= 0 && presentation >= 0; }
    static QueueFamilyIndices query(NativeRenderObject::Handle physicalDeviceHandle,
                                    NativeRenderObject::Handle surfaceHandle);
};

struct QueueSpecification {
    const LogicalDevice* logicalDevice;
    QueueType queueType;
    uint32 queueIndex;
};

class Queue : public NativeRenderObject {
public:
    void acquire(const QueueSpecification& spec);

    QueueType type() const { return m_spec.queueType; }
    uint32 index() const { return m_spec.queueIndex; }

private:
    QueueSpecification m_spec;
};

} // namespace R3

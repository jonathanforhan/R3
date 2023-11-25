#pragma once
#include "api/Types.hpp"
#include "render/NativeRenderObject.hpp"

namespace R3 {

struct QueueFamilyIndices {
    int32 graphics = -1;

    bool isValid() const { return graphics >= 0; }
};

class Queues {
public:
    static QueueFamilyIndices queryQueueFamilyIndices(NativeRenderObject::Handle physicalDeviceHandle);
};

} // namespace R3

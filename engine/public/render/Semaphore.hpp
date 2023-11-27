#pragma once

#include "render/NativeRenderObject.hpp"
#include "render/RenderFwd.hpp"

namespace R3 {
    
struct SemaphoreSpecification {
    const LogicalDevice* logicalDevice;
};

class Semaphore : public NativeRenderObject {
public:
    void create(const SemaphoreSpecification& spec);
    void destroy();

private:
    SemaphoreSpecification m_spec;
};

} // namespace R3

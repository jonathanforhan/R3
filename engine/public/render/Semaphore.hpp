#pragma once

#include "render/NativeRenderObject.hpp"
#include "render/RenderFwd.hpp"

namespace R3 {

struct SemaphoreSpecification {
    const LogicalDevice& logicalDevice;
};

class Semaphore : public NativeRenderObject {
public:
    Semaphore() = default;
    Semaphore(const SemaphoreSpecification& spec);
    Semaphore(Semaphore&&) noexcept = default;
    Semaphore& operator=(Semaphore&&) noexcept = default;
    ~Semaphore();

private:
    Ref<const LogicalDevice> m_logicalDevice;
};

} // namespace R3

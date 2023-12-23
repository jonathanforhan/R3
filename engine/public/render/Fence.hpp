#pragma once

#include "render/NativeRenderObject.hpp"
#include "render/RenderFwd.hpp"

namespace R3 {

struct FenceSpecification {
    const LogicalDevice& logicalDevice;
};

class Fence : public NativeRenderObject {
public:
    Fence() = default;
    Fence(const FenceSpecification& spec);
    Fence(Fence&&) noexcept = default;
    Fence& operator=(Fence&&) noexcept = default;
    ~Fence();

private:
    Ref<const LogicalDevice> m_logicalDevice;
};

} // namespace R3

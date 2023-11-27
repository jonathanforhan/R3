#pragma once

#include "render/NativeRenderObject.hpp"
#include "render/RenderFwd.hpp"

namespace R3 {

struct FenceSpecification {
    const LogicalDevice* logicalDevice;
};

class Fence : public NativeRenderObject {
public:
    void create(const FenceSpecification& spec);
    void destroy();

private:
    FenceSpecification m_spec;
};

} // namespace R3

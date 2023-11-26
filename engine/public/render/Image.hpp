#pragma once
#include <vector>
#include "render/NativeRenderObject.hpp"
#include "render/RenderFwd.hpp"

namespace R3 {

struct ImageSpecification {
    const LogicalDevice* logicalDevice;
    const Swapchain* swapchain;
};

class Image : public NativeRenderObject {
public:
    static std::vector<Image> acquireImages(const ImageSpecification& spec);
};

} // namespace R3
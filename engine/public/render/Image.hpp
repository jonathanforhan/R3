#pragma once

#include <vector>
#include "render/NativeRenderObject.hpp"
#include "render/RenderFwd.hpp"

namespace R3 {

struct ImageSpecification {
    Ref<const LogicalDevice> logicalDevice;
    Ref<const Swapchain> swapchain;
};

class Image : public NativeRenderObject {
public:
    static std::vector<Image> acquireImages(const ImageSpecification& spec);
};

} // namespace R3
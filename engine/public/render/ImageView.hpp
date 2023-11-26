#pragma once

#include <vector>
#include "render/NativeRenderObject.hpp"
#include "render/RenderFwd.hpp"

namespace R3 {

struct ImageViewSpecification {
    const LogicalDevice* logicalDevice;
    const Swapchain* swapchain;
    const Image* image;
};

class ImageView : public NativeRenderObject {
public:
    void create(const ImageViewSpecification& spec);
    void destroy();

private:
    ImageViewSpecification m_spec;
};

} // namespace R3
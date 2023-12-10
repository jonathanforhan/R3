#pragma once

#include <vector>
#include "render/NativeRenderObject.hpp"
#include "render/RenderFwd.hpp"

namespace R3 {

struct ImageViewSpecification {
    Ref<const LogicalDevice> logicalDevice;
    Ref<const Swapchain> swapchain;
    Ref<const Image> image;
};

class ImageView : public NativeRenderObject {
public:
    ImageView() = default;
    ImageView(const ImageViewSpecification& spec);
    ImageView(ImageView&&) noexcept = default;
    ImageView& operator=(ImageView&&) noexcept = default;
    ~ImageView();

private:
    ImageViewSpecification m_spec;
};

} // namespace R3
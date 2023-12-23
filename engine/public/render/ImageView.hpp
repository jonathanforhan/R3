#pragma once

#include "api/Types.hpp"
#include "render/NativeRenderObject.hpp"
#include "render/RenderFwd.hpp"
#include "render/RenderSpecification.hpp"

namespace R3 {

struct ImageViewSpecification {
    const LogicalDevice& logicalDevice;
    const Image& image;
    Format format;
    uint32 mipLevels;
    ImageAspect::Flags aspectMask;
};

class ImageView : public NativeRenderObject {
public:
    ImageView() = default;
    ImageView(const ImageViewSpecification& spec);
    ImageView(ImageView&&) noexcept = default;
    ImageView& operator=(ImageView&&) noexcept = default;
    ~ImageView();

private:
    Ref<const LogicalDevice> m_logicalDevice;
};

} // namespace R3
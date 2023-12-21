#pragma once

#include "api/Types.hpp"
#include "render/NativeRenderObject.hpp"
#include "render/RenderFwd.hpp"
#include "render/RenderSpecification.hpp"

namespace R3 {

struct ImageViewSpecification {
    Ref<const LogicalDevice> logicalDevice;
    Ref<const Image> image;
    Format format;
    uint64 aspectMask;
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
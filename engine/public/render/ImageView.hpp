#pragma once

/// @file ImageView.hpp
/// @brief Provides a view into and Image buffer

#include "render/RenderApi.hpp"

namespace R3 {

/// @brief Image View Specification
struct ImageViewSpecification {
    const LogicalDevice& logicalDevice; ///< LogicalDevice
    const Image& image;                 ///< Image
    Format format;                      ///< Image Format
    uint32 mipLevels;                   ///< Image miplevels
    ImageAspect::Flags aspectMask;      ///< Image aspect mask
};

/// @brief ImageView contains data about a child image
/// Images cannot be passed directly to the swapchain so ImageViews wrap them
class ImageView : public NativeRenderObject {
public:
    DEFAULT_CONSTRUCT(ImageView);
    NO_COPY(ImageView);
    DEFAULT_MOVE(ImageView);

    /// @brief Construct ImageView from spec
    /// @param spec
    ImageView(const ImageViewSpecification& spec);

    /// @brief Destroy ImageView (Images are not freed when doing this)
    ~ImageView();

private:
    Ref<const LogicalDevice> m_logicalDevice;
};

} // namespace R3
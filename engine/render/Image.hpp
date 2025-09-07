#pragma once

#include "Flags.hpp"
#include "RenderFwd.hpp"
#include "RenderHandle.hpp"
#include "engine/api/Api.hpp"
#include "engine/api/Class.hpp"
#include "engine/api/MovableHandle.hpp"
#include "engine/api/Types.hpp"

namespace R3 {

/// @brief Image class encapsulates an image and image-view with associated device memory management.
class R3_API Image {
public:
    R3_CTOR_DEFAULT(Image);
    R3_COPY_DELETE(Image);
    R3_MOVE_DEFAULT(Image);

    /// @brief Create an image with given properties.
    /// @param extent Dimensions of the image (width, height, depth).
    /// @param mipLevels Maximum number of mipmap levels.
    /// @param samples Number of samples per texel (for multisampling).
    /// @param usage Usage flags for the image (see ImageUsage).
    /// @param format Pixel format of the image (see Format).
    /// @param type Image type (1D, 2D, 3D, Cube).
    Image(usize3 extent,
          uint32 mipLevels,
          uint32 samples,
          ImageUsageFlags usage,
          Format format,
          ImageType type = ImageType::Image2D);

    /// @brief Destroy the image, its memory, and its image view.
    ~Image() noexcept;

    /// @brief Generate mipmaps for the image using the provided command buffer.
    /// @param cmd A reference to a command buffer that is currently recording commands.
    void generateMipmaps(ICommandBuffer& cmd);

    /// @return The 3D image extent in texels/pixels, represented as a usize3.
    usize3 extent() const noexcept { return m_extent; }
    /// @return Image mip levels.
    uint32 mipLevels() const noexcept { return m_mipLevels; }
    /// @return Image samples per texel.
    uint32 samples() const noexcept { return m_samples; }
    /// @return Image usage flags.
    ImageUsageFlags usage() const noexcept { return m_usage; }
    /// @return Image format.
    Format format() const noexcept { return m_format; }
    /// @return Bytes per pixel for the image format.
    uint32 bytesPerPixel() const noexcept { return formatPixelSize(m_format); }
    /// @return Image type.
    ImageType type() const noexcept { return m_type; }
    /// @return The number of array layers (6 for cube maps, 1 otherwise).
    uint32 layerCount() const noexcept { return m_type == ImageType::ImageCube ? 6U : 1U; }
    /// @return A reference to the underlying image handle.
    ImageRenderHandle& imageHandle() noexcept { return m_image; }
    /// @return A const reference to the underlying image handle.
    const ImageRenderHandle& imageHandle() const noexcept { return m_image; }
    /// @return A reference to the underlying image-view handle.
    ImageViewRenderHandle& imageViewHandle() noexcept { return m_imageView; }
    /// @return A const reference to the underlying image-view handle.
    const ImageViewRenderHandle& imageViewHandle() const noexcept { return m_imageView; }

private:
    MovableHandle<ImageRenderHandle> m_image;
    MovableHandle<ImageViewRenderHandle> m_imageView;
    MovableHandle<DeviceMemoryRenderHandle> m_memory;
    usize3 m_extent         = {0, 0, 0};
    uint32 m_mipLevels      = 1;
    uint32 m_samples        = 1;
    ImageUsageFlags m_usage = ImageUsageFlags(-1);
    Format m_format         = Format::Undefined;
    ImageType m_type        = ImageType::Image2D;
};

} // namespace R3
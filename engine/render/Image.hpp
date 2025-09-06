#pragma once

#include "RenderHandle.hpp"
#include "engine/api/Api.hpp"
#include "engine/api/Class.hpp"
#include "engine/api/MovableHandle.hpp"
#include "engine/api/Types.hpp"
#include "engine/render/Flags.hpp"

#include "engine/render/vulkan/vulkan-Fwd.hpp"

namespace R3 {

/// @brief Image class encapsulates an image, its memory, and its image view.
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
    ~Image();

    /// @brief Generate mipmaps for the image using the provided command buffer.
    /// @param cmd A reference to a command buffer that is currently recording commands.
    void generateMipMaps(vulkan::CommandBuffer& cmd);
    // ^^^ TODO make renderer agnostic

    /// @brief Get the underlying RenderHandle of the image as a T&.
    /// @tparam T Type to cast the handle to (e.g., VkImage)
    /// @return A reference to the underlying image handle.
    template <typename T>
    const T& handle() const noexcept {
        return m_image.get<T>();
    }

    /// @brief Get the underlying RenderHandle of the image-view as a T&.
    /// @tparam T Type to cast the handle to (e.g., VkImageView)
    /// @return A reference to the underlying image-view handle.
    template <typename T>
    const T& imageView() const noexcept {
        return m_imageView.get<T>();
    }

private:
    MovableHandle<RenderHandle> m_image;
    MovableHandle<RenderHandle> m_imageView;
    MovableHandle<RenderHandle> m_memory;
    usize3 m_extent         = {0, 0, 0};
    uint32 m_mipLevels      = 1;
    uint32 m_samples        = 1;
    ImageUsageFlags m_usage = ImageUsageFlags(-1);
    Format m_format         = Format::Undefined;
    ImageType m_type        = ImageType::Image2D;
};

} // namespace R3
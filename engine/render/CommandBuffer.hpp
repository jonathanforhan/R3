#pragma once

#include "RenderFwd.hpp"
#include "engine/api/Api.hpp"
#include "engine/api/Types.hpp"

namespace R3 {

class R3_API ICommandBuffer {
public:
    virtual ~ICommandBuffer() noexcept = default;

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// State Commands
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    /// @brief Begin recording commands to the command buffer.
    virtual void beginCommands() = 0;
    /// @brief Ends the current sequence of commands.
    virtual void endCommands() = 0;

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Transfer Commands
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    /// @brief Copies the contents of the source buffer to the destination buffer.
    /// @param src The source buffer to copy from.
    /// @param dst The destination buffer to copy to.
    virtual void copyBuffer(const Buffer& src, Buffer& dst) = 0;

    /// @brief Copies a specified number of bytes from a source buffer to a destination buffer.
    /// @param src The source buffer from which data will be copied.
    /// @param srcOffset The offset, in bytes, within the source buffer to start copying from.
    /// @param dst The destination buffer to which data will be copied.
    /// @param dstOffset The offset, in bytes, within the destination buffer to start copying to.
    /// @param size The number of bytes to copy.
    virtual void copyBuffer(const Buffer& src, usize srcOffset, Buffer& dst, usize dstOffset, usize size) = 0;

    /// @brief Copies the contents of the source image to the destination image.
    /// @param src The source image to copy from.
    /// @param dst The destination image to copy to.
    virtual void copyImage(const Image& src, Image& dst) = 0;

    /// @brief Copies a region of pixels from a source image to a destination image.
    /// @param src The source image from which pixels are copied.
    /// @param srcOffset The starting offset (x, y, z) in the source image.
    /// @param dst The destination image to which pixels are copied.
    /// @param dstOffset The starting offset (x, y, z) in the destination image.
    /// @param extent The size (width, height, depth) of the region to copy.
    virtual void copyImage(const Image& src, usize3 srcOffset, Image& dst, usize3 dstOffset, usize3 extent) = 0;

    /// @brief Copies data from a buffer to an image.
    /// @param src The source buffer containing the data to copy.
    /// @param dst The destination image to which the data will be copied.
    virtual void copyBufferToImage(const Buffer& src, Image& dst) = 0;

    /// @brief Copies data from a buffer to a region of an image.
    /// @param src The source buffer from which data will be copied.
    /// @param srcOffset The offset, in bytes, within the source buffer where copying begins.
    /// @param dst The destination image to which data will be copied.
    /// @param dstOffset The offset, in pixels, within the destination image where data will be placed.
    /// @param dstExtent The size, in pixels, of the region in the destination image to copy data into.
    virtual void copyBufferToImage(const Buffer& src,
                                   usize srcOffset,
                                   Image& dst,
                                   usize3 dstOffset,
                                   usize3 dstExtent) = 0;

    /// @brief Copies image data from a source image to a destination buffer.
    /// @param src The source image to copy from.
    /// @param dst The destination buffer to copy the image data into.
    virtual void copyImageToBuffer(const Image& src, Buffer& dst) = 0;

    /// @brief Copies a region of an image to a buffer.
    /// @param src The source image from which to copy data.
    /// @param srcOffset The offset (x, y, z) in the source image where copying begins.
    /// @param srcExtent The size (width, height, depth) of the region to copy from the source image.
    /// @param dst The destination buffer to which the image data will be copied.
    /// @param dstOffset The offset in the destination buffer where the copied data will be placed.
    virtual void copyImageToBuffer(const Image& src,
                                   usize3 srcOffset,
                                   usize3 srcExtent,
                                   Buffer& dst,
                                   usize dstOffset) = 0;
};

} // namespace R3
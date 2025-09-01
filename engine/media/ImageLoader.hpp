#pragma once

#include <cstddef>
#include <filesystem>
#include <memory>
#include "engine/api/Class.hpp"
#include "engine/api/Types.hpp"

namespace R3 {

class ImageLoader {
public:
    using UniqueImageData = std::unique_ptr<std::byte, void (*)(void*)>;

    struct ImageDescriptor {
        R3_CTOR_DEFAULT(ImageDescriptor);
        R3_COPY_DELETE(ImageDescriptor);
        R3_MOVE_DEFAULT(ImageDescriptor);

        ImageDescriptor(std::byte* data, usize width, usize height, uint32 channels);

        UniqueImageData data{nullptr, nullptr};
        usize width     = 0;
        usize height    = 0;
        uint32 channels = 0;
    };

    /// @brief Load an image from filesystem
    /// @param path Path to image file
    /// @param reqChannels Desired channels (e.g. 3 = RGB, 4 = RGBA), 0 to load as-is
    /// @return Image descriptor
    static ImageDescriptor loadImageFile(const std::filesystem::path& path, uint32 reqChannels = 0);

    /// @brief Load an image from compressed data
    /// @param compressed Pointer to compressed image data in memory
    /// @param size Size of image in bytes
    /// @param reqChannels Desired channels (e.g. 3 = RGB, 4 = RGBA), 0 to load as-is
    /// @return Image descriptor
    static ImageDescriptor loadImageCompressed(const std::byte* compressed, usize size, uint32 reqChannels = 0);
};

} // namespace R3
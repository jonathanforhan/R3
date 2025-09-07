#include "ImageLoader.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <cstddef>
#include <filesystem>
#include <string>
#include "api/Assert.hpp"
#include "api/Types.hpp"

namespace R3 {

ImageLoader::ImageDescriptor::ImageDescriptor(std::byte* data, usize width, usize height, uint32 channels)
    : data(data, stbi_image_free),
      width(width),
      height(height),
      channels(channels) {}

ImageLoader::ImageDescriptor ImageLoader::loadImageFile(const std::filesystem::path& path, uint32 reqChannels) {
    R3_ASSERT(std::filesystem::exists(path), "Ensure valid filepath");

    int w, h, ch;
    auto* p = stbi_load(path.string().c_str(), &w, &h, &ch, (int)reqChannels);
    R3_ASSERT(p, "failed to load image");

    return ImageDescriptor{
        (std::byte*)p,
        static_cast<usize>(w),
        static_cast<usize>(h),
        static_cast<uint32>(reqChannels == 0 ? ch : reqChannels),
    };
}

ImageLoader::ImageDescriptor ImageLoader::loadImageCompressed(const std::byte* compressed,
                                                              usize size,
                                                              uint32 reqChannels) {
    int w, h, ch;
    auto* p = stbi_load_from_memory((const uint8*)compressed, (int)size, &w, &h, &ch, (int)reqChannels);
    R3_ASSERT(p, "failed to load image");

    return ImageDescriptor{
        (std::byte*)p,
        static_cast<usize>(w),
        static_cast<usize>(h),
        static_cast<uint32>(ch),
    };
}

} // namespace R3

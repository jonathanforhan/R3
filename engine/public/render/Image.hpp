#pragma once

#include <vector>
#include "api/Types.hpp"
#include "render/NativeRenderObject.hpp"
#include "render/RenderFwd.hpp"
#include "render/RenderSpecification.hpp"

namespace R3 {

struct ImageSpecification {
    Ref<const LogicalDevice> logicalDevice;
    Ref<const Swapchain> swapchain;
};

struct ImageAllocateSpecification {
    const PhysicalDevice& physicalDevice;
    const LogicalDevice& logicalDevice;
    usize size;
    Format format;
    uint32 width;
    uint32 height;
    uint64 imageFlags;
    uint64 memoryFlags;
};

enum class ImageCopyType {
    Image,
    BufferToImage,
};

struct ImageCopySpecification {
    const LogicalDevice& logicalDevice;
    const CommandPool& commandPool;
    NativeRenderObject& dst;
    const NativeRenderObject& src;
    usize size;
    uint32 width;
    uint32 height;
    ImageCopyType copyType;
};

class Image : public NativeRenderObject {
public:
    static std::vector<Image> acquireImages(const ImageSpecification& spec);

    /// @brief Allocate a Image with given flags
    /// @param spec 
    /// @return tuple<Image::Handle, DeviceMemory::Handle>
    [[nodiscard]] static std::tuple<NativeRenderObject, NativeRenderObject> allocate(
        const ImageAllocateSpecification& spec);

    static void copy(const ImageCopySpecification& spec);
};

} // namespace R3
#pragma once

/// @brief Encompasses all Image operations like allocateion, copy, transition etc

#include <vector>
#include "render/RenderApi.hpp"

namespace R3 {

/// @brief Image Specification
struct R3_API ImageSpecification {
    const LogicalDevice& logicalDevice; ///< LogicalDevice
    const Swapchain& swapchain;         ///< Swapchain
};

/// @brief Image Allocate Specification
struct R3_API ImageAllocateSpecification {
    const PhysicalDevice& physicalDevice; ///< PhysicalDevice
    const LogicalDevice& logicalDevice;   ///< LogicalDevice
    usize size;                           ///< Image size in bytes of Image
    Format format;                        ///< Image Format
    uint32 width;                         ///< Image width
    uint32 height;                        ///< Image height
    uint32 mipLevels = 1;                 ///< Image Mipmap Levels
    uint8 samples;                        ///< Image Samples (1, 2, 4, ... 64)
    ImageUsage::Flags imageFlags;         ///< Usage flags
    MemoryProperty::Flags memoryFlags;    ///< Memory flags
};

/// @brief Image Copy Specification
struct R3_API ImageCopySpecification {
    const LogicalDevice& logicalDevice; ///< LogicalDevice
    const CommandBuffer& commandBuffer; ///< CommandBuffer
    NativeRenderObject& dst;            ///< NativeRenderObject
    const NativeRenderObject& src;      ///< NativeRenderObject
    usize size;                         ///< Image size in bytes
    uint32 width;                       ///< Image width
    uint32 height;                      ///< Image height
    ImageCopyType copyType;             ///< Image copy details
};

/// @brief Image Layout Transition Specification
/// Transition the Image Layout, this is recorded by the CommandBuffer and performed in the Pipeline Stages
/// The CommandBuffer will perform a blocking one-time submit on copy
struct R3_API ImageLayoutTransitionSpecification {
    const CommandBuffer& commandBuffer; ///< CommandBuffer
    Image& image;                       ///< Mutable Image
    MemoryAccessor::Flags srcAccessor;  ///< Source Accessor
    MemoryAccessor::Flags dstAccessor;  ///< Destination Accessor
    ImageLayout oldLayout;              ///< Old ImageLayout
    ImageLayout newLayout;              ///< New ImageLayout
    ImageAspect::Flags aspectMask;      ///< Aspect
    uint32 mipLevels = 1;               ///< Mipmap levels
    uint32 baseMipLevel = 0;            ///< Base mipmap level (default=0)
    PipelineStage::Flags srcStageMask;  ///< Source Stage Mask
    PipelineStage::Flags dstStageMask;  ///< Destination Stage Mask
};

/// @brief Image Mipmap Specification
struct R3_API ImageMipmapSpecification {
    const CommandBuffer& commandBuffer; ///< CommandBuffer
    Image& image;                       ///< Mutable Image
    uint32 mipLevels;                   ///< Mipmap levels
    int32 width;                        ///< Image width
    int32 height;                       ///< Image height
};

/// @brief Image represents an allocated Image. Images can only be accessed with ImageViews
class R3_API Image : public NativeRenderObject {
public:
    DEFAULT_CONSTRUCT(Image);
    NO_COPY(Image);
    DEFAULT_MOVE(Image);

    /// @brief Construct an Image Wrapper from native id
    /// Used for passing raw handle to the different image operations
    /// @param nativeRenderObject
    explicit Image(const NativeRenderObject& nativeRenderObject) { setHandle(nativeRenderObject.handle()); }

    /// @brief Construct set of Images from spec
    /// @param spec
    /// @return Images
    [[nodiscard]] static std::vector<Image> acquireImages(const ImageSpecification& spec);

    /// @brief Allocate a Image with given flags
    /// @param spec
    /// @return tuple<Image::Handle, DeviceMemory::Handle>
    [[nodiscard]] static auto allocate(const ImageAllocateSpecification& spec)
        -> std::tuple<NativeRenderObject, NativeRenderObject>;

    /// @brief Copy Image from dst to src using given spec
    /// @param spec
    static void copy(const ImageCopySpecification& spec);

    /// @brief Transition ImageLayout from spec
    /// @param spec
    static void transition(const ImageLayoutTransitionSpecification& spec);

    /// @brief Generate Mipmaps with CommandBuffer blitting
    /// @param spec
    static void generateMipMaps(const ImageMipmapSpecification& spec);
};

} // namespace R3
#pragma once

/// @file DepthBuffer.hxx

#include "render/Buffer.hxx"
#include "render/ImageView.hxx"
#include "render/RenderApi.hxx"

namespace R3 {

/// @brief Depth Buffer Specification
struct DepthBufferSpecification {
    const PhysicalDevice& physicalDevice; ///< PhysicalDevice
    const LogicalDevice& logicalDevice;   ///< LogicalDevice
    const Swapchain& swapchain;           ///< Swapchain
};

/// @brief DepthBuffer (or Z-Buffer) is used for Depth Operations
class DepthBuffer : public Buffer {
public:
    DEFAULT_CONSTRUCT(DepthBuffer);
    NO_COPY(DepthBuffer);
    DEFAULT_MOVE(DepthBuffer);

    /// @brief Construct DepthBuffer from spec
    /// @param spec
    DepthBuffer(const DepthBufferSpecification& spec);

    /// @brief Free Buffer and it's DeviceMemory
    ~DepthBuffer();

    /// @brief Query DepthBuffer ImageViews
    /// @return image views
    const ImageView& imageView() const { return m_imageView; }

private:
    Ref<const LogicalDevice> m_logicalDevice;
    ImageView m_imageView;
};

} // namespace R3

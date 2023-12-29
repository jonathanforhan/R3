#pragma once

/// @file ColorBuffer.hxx

#include "render/Buffer.hxx"
#include "render/ImageView.hxx"
#include "render/RenderApi.hxx"

namespace R3 {

/// @brief Color Buffer Specification
struct ColorBufferSpecification {
    const PhysicalDevice& physicalDevice; ///< PhysicalDevice
    const LogicalDevice& logicalDevice;   ///< LogicalDevice
    const Swapchain& swapchain;           ///< Swapchain
};

/// @brief ColorBuffer used for Color Operations like Mulitdsampling
class ColorBuffer : public Buffer {
public:
    DEFAULT_CONSTRUCT(ColorBuffer);
    NO_COPY(ColorBuffer);
    DEFAULT_MOVE(ColorBuffer);

    /// @brief Construct ColorBuffer from spec
    /// @param spec
    ColorBuffer(const ColorBufferSpecification& spec);

    /// @brief Free Buffer and it's DeviceMemory
    ~ColorBuffer();

    /// @brief Query ColorBuffer ImageViews
    /// @return image views
    const ImageView& imageView() const { return m_imageView; }

private:
    Ref<const LogicalDevice> m_logicalDevice;
    ImageView m_imageView;
};

} // namespace R3

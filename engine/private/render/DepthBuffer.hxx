#pragma once

#include "render/Buffer.hxx"
#include "render/ImageView.hxx"
#include "render/RenderApi.hxx"

namespace R3 {

/// @brief Depth Buffer Specification
struct DepthBufferSpecification {
    const PhysicalDevice& physicalDevice;
    const LogicalDevice& logicalDevice;
    uvec2 extent;      ///< Image extent
    uint8 sampleCount; ///< MSAA samples
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
    [[nodiscard]] constexpr const ImageView& imageView() const { return m_imageView; }

    /// @brief Query Current Format in use
    /// @return format
    [[nodiscard]] constexpr Format format() const { return m_format; }

    /// @brief Query Current Extent in use
    /// @return extent
    [[nodiscard]] constexpr uvec2 extent() const { return m_extent; }

    /// @brief Query Current Sample Count in use
    /// @return sample count
    [[nodiscard]] constexpr uint8 sampleCount() const { return m_sampleCount; }

private:
    Ref<const LogicalDevice> m_logicalDevice;
    ImageView m_imageView;
    Format m_format = Format::Undefined;
    uvec2 m_extent = uvec2(0);
    uint8 m_sampleCount = 1;
};

} // namespace R3

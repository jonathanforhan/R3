#pragma once

#include "render/Buffer.hpp"
#include "render/ImageView.hpp"
#include "render/RenderApi.hpp"

namespace R3 {

/// @brief Color Buffer Specification
struct R3_API ColorBufferSpecification {
    const PhysicalDevice& physicalDevice;
    const LogicalDevice& logicalDevice;
    Format format;
    uvec2 extent;      ///< Image extent
    uint8 sampleCount; ///< MSAA samples
};

/// @brief ColorBuffer used for Color Operations like Mulitdsampling
class R3_API ColorBuffer : public Buffer {
public:
    DEFAULT_CONSTRUCT(ColorBuffer);
    NO_COPY(ColorBuffer);
    DEFAULT_MOVE(ColorBuffer);

    /// @brief Construct ColorBuffer from spec
    /// @param spec
    ColorBuffer(const ColorBufferSpecification& spec);

    /// @brief Free Buffer and it's DeviceMemory
    ~ColorBuffer();

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

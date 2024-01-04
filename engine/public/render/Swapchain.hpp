#pragma once

#include "render/Image.hpp"
#include "render/ImageView.hpp"
#include "render/RenderApi.hpp"

namespace R3 {

/// @brief Swapchain Specification
struct R3_API SwapchainSpecification {
    const PhysicalDevice& physicalDevice; ///< PhysicalDevice
    const Surface& surface;               ///< Surface
    const LogicalDevice& logicalDevice;   ///< LogicalDevice
    const Window& window;                 ///< Window
};

/// @brief Swapchain Recreation Specification
struct R3_API SwapchainRecreationSpecification {
    std::vector<Framebuffer>& framebuffers; ///< Framebuffers to be recreated
    ColorBuffer& colorBuffer;               ///< ColorBuffer to recreate
    DepthBuffer& depthBuffer;               ///< DepthBuffer to recreate
    const RenderPass& renderPass;           ///< RenderPass to reference
};

/// @brief Swapchain is an Abstract that represents and series of images
/// We do not own these images, we only look into them with ImageViews
class R3_API Swapchain : public NativeRenderObject {
public:
    DEFAULT_CONSTRUCT(Swapchain);
    NO_COPY(Swapchain);
    DEFAULT_MOVE(Swapchain);

    /// @brief Create a Swapchain, queury device for images, create ImageViews from them
    /// @param spec
    Swapchain(const SwapchainSpecification& spec);

    /// @brief Free image views and swapchain the actual images were never ours so we don't touch them
    ~Swapchain();

    /// @brief A Swapchain may become unusable (like a window resize) or suboptimal so we recreate it
    /// @param spec
    void recreate(const SwapchainRecreationSpecification& spec);

    [[nodiscard]] constexpr Format surfaceFormat() const { return m_surfaceFormat; }
    [[nodiscard]] constexpr ColorSpace colorSpace() const { return m_colorSpace; }
    [[nodiscard]] constexpr PresentMode presentMode() const { return m_presentMode; }
    [[nodiscard]] constexpr uvec2 extent() const { return m_extent; }
    [[nodiscard]] constexpr std::span<const Image> images() const { return m_images; }
    [[nodiscard]] constexpr std::span<const ImageView> imageViews() const { return m_imageViews; }

private:
    Ref<const PhysicalDevice> m_physicalDevice;
    Ref<const Surface> m_surface;
    Ref<const LogicalDevice> m_logicalDevice;
    Ref<const Window> m_window;

    Format m_surfaceFormat = Format::Undefined;
    ColorSpace m_colorSpace = ColorSpace::SrgbNonlinear;
    PresentMode m_presentMode = PresentMode::Fifo;
    uvec2 m_extent = uvec2(0);
    uint32 m_imageCount = 0;

    std::vector<Image> m_images;
    std::vector<ImageView> m_imageViews;
};

} // namespace R3
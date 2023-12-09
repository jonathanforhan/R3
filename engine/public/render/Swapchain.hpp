#pragma once

#include <vector>
#include "render/Image.hpp"
#include "render/ImageView.hpp"
#include "render/NativeRenderObject.hpp"
#include "render/RenderFwd.hpp"
#include "render/RenderSpecification.hpp"

namespace R3 {

/// @brief Swapchain Specification
struct SwapchainSpecification {
    const PhysicalDevice* physicalDevice; ///< @brief Valid non-null PhysicalDevice
    const Surface* surface;               ///< @brief Valid non-null Surface
    const LogicalDevice* logicalDevice;   ///< @brief Valid non-null LogicalDevice
    const Window* window;                 ///< @brief Valid non-null Window
};

/// @brief Swapchain is an Abstract that represents and series of images
/// We do not own these images, we only look into them with ImageViews
class Swapchain : public NativeRenderObject {
public:
    /// @brief Create a Swapchain, queury device for images, create ImageViews from them
    /// @param spec 
    void create(const SwapchainSpecification& spec);

    /// @brief A Swapchain may become unusable (like a window resize) or suboptimal so we recreate it
    /// @param framebuffers list of Framebuffers
    /// @param renderPass current render pass
    void recreate(std::vector<Framebuffer>& framebuffers, const RenderPass& renderPass);

    /// @brief Free image views and swapchain the actual images were never ours so we don't touch them
    void destroy();

    Format surfaceFormat() const { return m_surfaceFormat; }                  ///< @brief Query surface format
    ColorSpace colorSpace() const { return m_colorSpace; }                    ///< @brief Query color space
    PresentMode presentMode() const { return m_presentMode; }                 ///< @brief Query present mode
    uvec2 extent() const { return m_extent2D; }                               ///< @brief Query swapchain extent
    const std::vector<Image>& images() const { return m_images; }             ///< @brief Query images
    const std::vector<ImageView>& imageViews() const { return m_imageViews; } ///< @brief Query image views

private:
    SwapchainSpecification m_spec;

    Format m_surfaceFormat = R3_FORMAT_UNDEFINED;
    ColorSpace m_colorSpace = R3_COLOR_SPACE_SRGB_NONLINEAR;
    PresentMode m_presentMode = R3_PRESENT_MODE_FIFO;
    uvec2 m_extent2D = uvec2{0U, 0U};
    uint32 m_imageCount = 0;

    std::vector<Image> m_images;
    std::vector<ImageView> m_imageViews;
};

} // namespace R3
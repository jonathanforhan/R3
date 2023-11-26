#pragma once
#include "api/Types.hpp"
#include "render/Image.hpp"
#include "render/ImageView.hpp"
#include "render/NativeRenderObject.hpp"
#include "render/RenderSpecification.hpp"

namespace R3 {

class PhysicalDevice;
class Surface;
class LogicalDevice;
class Window;

struct SwapchainSpecification {
    const PhysicalDevice* physicalDevice;
    const Surface* surface;
    const LogicalDevice* logicalDevice;
    const Window* window;
};

class Swapchain : public NativeRenderObject {
public:
    void create(const SwapchainSpecification& spec);
    void destroy();

    Format surfaceFormat() const { return m_surfaceFormat; }
    ColorSpace colorSpace() const { return m_colorSpace; }
    PresentMode presentMode() const { return m_presentMode; }
    const std::vector<Image>& images() const { return m_images; }

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
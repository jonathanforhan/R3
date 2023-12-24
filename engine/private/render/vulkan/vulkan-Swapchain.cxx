#if R3_VULKAN

#include "render/Swapchain.hpp"

// clang-format off
#include <vulkan/vulkan.hpp>
#include <GLFW/glfw3.h>
// clang-format on
#include "api/Check.hpp"
#include "api/Log.hpp"
#include "render/ColorBuffer.hpp"
#include "render/DepthBuffer.hpp"
#include "render/Framebuffer.hpp"
#include "render/LogicalDevice.hpp"
#include "render/PhysicalDevice.hpp"
#include "render/Surface.hpp"
#include "render/Window.hpp"
#include "vulkan-SwapchainSupportDetails.hxx"

namespace R3 {

Swapchain::Swapchain(const SwapchainSpecification& spec)
    : m_physicalDevice(&spec.physicalDevice),
      m_surface(&spec.surface),
      m_logicalDevice(&spec.logicalDevice),
      m_window(&spec.window) {
    const auto swapchainSupportDetails = vulkan::SwapchainSupportDetails::query(
        m_physicalDevice->as<vk::PhysicalDevice>(), m_surface->as<vk::SurfaceKHR>());

    const auto format = swapchainSupportDetails.optimalSurfaceFormat();
    m_surfaceFormat = std::get<0>(format);
    m_colorSpace = std::get<1>(format);
    m_extent2D = swapchainSupportDetails.optimalExtent(m_window->handle<GLFWwindow*>());

    swapchainSupportDetails.capabilities.minImageCount == swapchainSupportDetails.capabilities.maxImageCount
        ? m_imageCount = swapchainSupportDetails.capabilities.maxImageCount
        : m_imageCount = swapchainSupportDetails.capabilities.minImageCount + 1;

    const uint32 queueFamilyIndices[] = {
        m_logicalDevice->graphicsQueue().index(),
        m_logicalDevice->presentationQueue().index(),
    };
    const bool sameQueueFamily = queueFamilyIndices[0] == queueFamilyIndices[1];

    const vk::SwapchainCreateInfoKHR swapchainCreateInfo = {
        .sType = vk::StructureType::eSwapchainCreateInfoKHR,
        .pNext = nullptr,
        .flags = {},
        .surface = m_surface->as<vk::SurfaceKHR>(),
        .minImageCount = m_imageCount,
        .imageFormat = (vk::Format)m_surfaceFormat,
        .imageColorSpace = (vk::ColorSpaceKHR)m_colorSpace,
        .imageExtent = vk::Extent2D(m_extent2D.x, m_extent2D.y),
        .imageArrayLayers = 1,
        .imageUsage = vk::ImageUsageFlagBits::eColorAttachment,
        .imageSharingMode = sameQueueFamily ? vk::SharingMode::eExclusive : vk::SharingMode::eConcurrent,
        .queueFamilyIndexCount = 2,                // NOTE does nothing if VK_SHARING_MODE_EXCLUSIVE is true
        .pQueueFamilyIndices = queueFamilyIndices, // NOTE does nothing if VK_SHARING_MODE_EXCLUSIVE is true
        .preTransform = swapchainSupportDetails.capabilities.currentTransform,
        .compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque,
        .presentMode = (vk::PresentModeKHR)m_presentMode,
        .clipped = VK_TRUE,
        .oldSwapchain = VK_NULL_HANDLE,
    };

    setHandle(m_logicalDevice->as<vk::Device>().createSwapchainKHR(swapchainCreateInfo));

    m_images = Image::acquireImages({
        .logicalDevice = *m_logicalDevice,
        .swapchain = *this,
    });

    m_imageViews.resize(m_images.size());
    for (usize i = 0; i < m_images.size(); i++) {
        m_imageViews[i] = ImageView({
            .logicalDevice = *m_logicalDevice,
            .image = m_images[i],
            .format = Format::R8G8B8A8Srgb,
            .mipLevels = 1,
            .aspectMask = ImageAspect::Color,
        });
    }
}

void Swapchain::recreate(const SwapchainRecreatationSpecification& spec) {
    CHECK(spec.framebuffers.size() == m_imageViews.size());
    m_logicalDevice->as<vk::Device>().waitIdle();

    vk::PhysicalDevice vkPhysicalDevice = m_physicalDevice->as<vk::PhysicalDevice>();
    vk::SurfaceKHR vkSurface = m_surface->as<vk::SurfaceKHR>();

    // query
    auto swapchainSupportDetails = vulkan::SwapchainSupportDetails::query(vkPhysicalDevice, vkSurface);
    m_extent2D = swapchainSupportDetails.optimalExtent(m_window->handle<GLFWwindow*>());

    // if extent == 0 -> we're minimized -> wait idle until maximized
    while (m_extent2D.x == 0 || m_extent2D.y == 0) {
        glfwWaitEvents();
        swapchainSupportDetails = vulkan::SwapchainSupportDetails::query(vkPhysicalDevice, vkSurface);
        m_extent2D = swapchainSupportDetails.optimalExtent(m_window->handle<GLFWwindow*>());
    }

    const uint32 queueFamilyIndices[] = {
        m_logicalDevice->graphicsQueue().index(),
        m_logicalDevice->presentationQueue().index(),
    };
    const bool sameQueueFamily = queueFamilyIndices[0] == queueFamilyIndices[1];

    // store old
    const vk::SwapchainKHR oldSwapchain = as<vk::SwapchainKHR>();

    const vk::SwapchainCreateInfoKHR swapchainCreateInfo = {
        .sType = vk::StructureType::eSwapchainCreateInfoKHR,
        .pNext = nullptr,
        .flags = {},
        .surface = m_surface->as<vk::SurfaceKHR>(),
        .minImageCount = m_imageCount,
        .imageFormat = (vk::Format)m_surfaceFormat,
        .imageColorSpace = (vk::ColorSpaceKHR)m_colorSpace,
        .imageExtent = vk::Extent2D(m_extent2D.x, m_extent2D.y),
        .imageArrayLayers = 1,
        .imageUsage = vk::ImageUsageFlagBits::eColorAttachment,
        .imageSharingMode = sameQueueFamily ? vk::SharingMode::eExclusive : vk::SharingMode::eConcurrent,
        .queueFamilyIndexCount = 2,                // NOTE does nothing if VK_SHARING_MODE_EXCLUSIVE is true
        .pQueueFamilyIndices = queueFamilyIndices, // NOTE does nothing if VK_SHARING_MODE_EXCLUSIVE is true
        .preTransform = vk::SurfaceTransformFlagBitsKHR::eIdentity,
        .compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque,
        .presentMode = (vk::PresentModeKHR)m_presentMode,
        .clipped = vk::True,
        .oldSwapchain = oldSwapchain,
    };

    setHandle(m_logicalDevice->as<vk::Device>().createSwapchainKHR(swapchainCreateInfo));
    m_logicalDevice->as<vk::Device>().destroySwapchainKHR(oldSwapchain);

    // recreate color buffer with new extent
    spec.colorBuffer.~ColorBuffer();
    spec.colorBuffer = ColorBuffer({
        .physicalDevice = *m_physicalDevice,
        .logicalDevice = *m_logicalDevice,
        .swapchain = *this,
    });

    // recreate depth buffer with new extent
    spec.depthBuffer.~DepthBuffer();
    spec.depthBuffer = DepthBuffer({
        .physicalDevice = *m_physicalDevice,
        .logicalDevice = *m_logicalDevice,
        .swapchain = *this,
    });

    // restore images
    m_images = Image::acquireImages({
        .logicalDevice = *m_logicalDevice,
        .swapchain = *this,
    });

    // recreate image views and framebuffers with new images
    m_imageViews.clear();
    spec.framebuffers.clear();

    for (usize i = 0; i < m_images.size(); i++) {
        m_imageViews.emplace_back(ImageViewSpecification{
            .logicalDevice = *m_logicalDevice,
            .image = m_images[i],
            .format = Format::R8G8B8A8Srgb,
            .mipLevels = 1,
            .aspectMask = ImageAspect::Color,
        });

        spec.framebuffers.emplace_back(FramebufferSpecification{
            .logicalDevice = *m_logicalDevice,
            .swapchain = *this,
            .swapchainImageView = m_imageViews[i],
            .colorBufferImageView = spec.colorBuffer.imageView(),
            .depthBufferImageView = spec.depthBuffer.imageView(),
            .renderPass = spec.renderPass,
        });
    }
}

Swapchain::~Swapchain() {
    if (validHandle()) {
        m_logicalDevice->as<vk::Device>().destroySwapchainKHR(as<vk::SwapchainKHR>());
    }
}

} // namespace R3

#endif // R3_VULKAN
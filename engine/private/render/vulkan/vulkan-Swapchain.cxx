#if R3_VULKAN

#include "render/Swapchain.hxx"

// clang-format off
#include <vulkan/vulkan.hpp>
#include <GLFW/glfw3.h>
// clang-format on
#include "api/Check.hpp"
#include "api/Log.hpp"
#include "render/ColorBuffer.hxx"
#include "render/DepthBuffer.hxx"
#include "render/Framebuffer.hxx"
#include "render/LogicalDevice.hxx"
#include "render/PhysicalDevice.hxx"
#include "render/Surface.hxx"
#include "render/Window.hxx"
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
    m_presentMode = swapchainSupportDetails.optimalPresentMode();
    m_extent = swapchainSupportDetails.optimalExtent(m_window->handle<GLFWwindow*>());

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
        .imageFormat = vk::Format(m_surfaceFormat),
        .imageColorSpace = vk::ColorSpaceKHR(m_colorSpace),
        .imageExtent = vk::Extent2D(m_extent.x, m_extent.y),
        .imageArrayLayers = 1,
        .imageUsage = vk::ImageUsageFlagBits::eColorAttachment,
        .imageSharingMode = sameQueueFamily ? vk::SharingMode::eExclusive : vk::SharingMode::eConcurrent,
        .queueFamilyIndexCount = 2,                // NOTE does nothing if VK_SHARING_MODE_EXCLUSIVE is true
        .pQueueFamilyIndices = queueFamilyIndices, // NOTE does nothing if VK_SHARING_MODE_EXCLUSIVE is true
        .preTransform = swapchainSupportDetails.capabilities.currentTransform,
        .compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque,
        .presentMode = vk::PresentModeKHR(m_presentMode),
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

void Swapchain::recreate(const SwapchainRecreationSpecification& spec) {
    CHECK(spec.framebuffers.size() == m_imageViews.size());
    m_logicalDevice->as<vk::Device>().waitIdle();

    vk::PhysicalDevice vkPhysicalDevice = m_physicalDevice->as<vk::PhysicalDevice>();
    vk::SurfaceKHR vkSurface = m_surface->as<vk::SurfaceKHR>();

    // query
    auto swapchainSupportDetails = vulkan::SwapchainSupportDetails::query(vkPhysicalDevice, vkSurface);
    m_extent = swapchainSupportDetails.optimalExtent(m_window->handle<GLFWwindow*>());

    // if extent == 0 -> we're minimized -> wait idle until maximized
    while (m_extent.x == 0 || m_extent.y == 0) {
        glfwWaitEvents();
        swapchainSupportDetails = vulkan::SwapchainSupportDetails::query(vkPhysicalDevice, vkSurface);
        m_extent = swapchainSupportDetails.optimalExtent(m_window->handle<GLFWwindow*>());
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
        .imageFormat = vk::Format(m_surfaceFormat),
        .imageColorSpace = vk::ColorSpaceKHR(m_colorSpace),
        .imageExtent = vk::Extent2D(m_extent.x, m_extent.y),
        .imageArrayLayers = 1,
        .imageUsage = vk::ImageUsageFlagBits::eColorAttachment,
        .imageSharingMode = sameQueueFamily ? vk::SharingMode::eExclusive : vk::SharingMode::eConcurrent,
        .queueFamilyIndexCount = 2,                // NOTE does nothing if VK_SHARING_MODE_EXCLUSIVE is true
        .pQueueFamilyIndices = queueFamilyIndices, // NOTE does nothing if VK_SHARING_MODE_EXCLUSIVE is true
        .preTransform = vk::SurfaceTransformFlagBitsKHR::eIdentity,
        .compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque,
        .presentMode = vk::PresentModeKHR(m_presentMode),
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
        .format = spec.colorBuffer.format(),
        .extent = m_extent,
        .sampleCount = spec.colorBuffer.sampleCount(),
    });

    // recreate depth buffer with new extent
    spec.depthBuffer.~DepthBuffer();
    spec.depthBuffer = DepthBuffer({
        .physicalDevice = *m_physicalDevice,
        .logicalDevice = *m_logicalDevice,
        .extent = m_extent,
        .sampleCount = spec.depthBuffer.sampleCount(),
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

        std::array<const ImageView*, 3> attachments = {
            &spec.colorBuffer.imageView(),
            &spec.depthBuffer.imageView(),
            &m_imageViews[i],
        };
        spec.framebuffers.emplace_back(FramebufferSpecification{
            .logicalDevice = *m_logicalDevice,
            .renderPass = spec.renderPass,
            .attachments = attachments,
            .extent = m_extent,
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
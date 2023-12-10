#if R3_VULKAN

#include "render/Swapchain.hpp"

// clang-format off
#include <vulkan/vulkan.hpp>
#include <GLFW/glfw3.h>
// clang-format on
#include <vector>
#include "api/Check.hpp"
#include "api/Ensure.hpp"
#include "render/Framebuffer.hpp"
#include "render/LogicalDevice.hpp"
#include "render/PhysicalDevice.hpp"
#include "render/Surface.hpp"
#include "render/Window.hpp"
#include "vulkan-SwapchainSupportDetails.hxx"

namespace R3 {

Swapchain::Swapchain(const SwapchainSpecification& spec)
    : m_spec(spec) {

    auto swapchainSupportDetails = vulkan::SwapchainSupportDetails::query(
        m_spec.physicalDevice->handle<VkPhysicalDevice>(), m_spec.surface->handle<VkSurfaceKHR>());

    auto format = swapchainSupportDetails.optimalSurfaceFormat();
    m_surfaceFormat = std::get<0>(format);
    m_colorSpace = std::get<1>(format);
    m_extent2D = swapchainSupportDetails.optimalExtent(m_spec.window->handle<GLFWwindow*>());

    swapchainSupportDetails.capabilities.minImageCount == swapchainSupportDetails.capabilities.maxImageCount
        ? m_imageCount = swapchainSupportDetails.capabilities.maxImageCount
        : m_imageCount = swapchainSupportDetails.capabilities.minImageCount + 1;

    uint32 queueFamilyIndices[] = {
        m_spec.logicalDevice->graphicsQueue().index(),
        m_spec.logicalDevice->presentationQueue().index(),
    };
    bool sameQueueFamily = queueFamilyIndices[0] == queueFamilyIndices[1];

    vk::SwapchainCreateInfoKHR swapchainCreateInfo = {
        .sType = vk::StructureType::eSwapchainCreateInfoKHR,
        .pNext = nullptr,
        .flags = {},
        .surface = m_spec.surface->handle<VkSurfaceKHR>(),
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

    setHandle(m_spec.logicalDevice->as<vk::Device>().createSwapchainKHR(swapchainCreateInfo));

    m_images = Image::acquireImages({
        .logicalDevice = m_spec.logicalDevice,
        .swapchain = this,
    });

    m_imageViews.resize(m_images.size());
    for (usize i = 0; i < m_images.size(); i++) {
        m_imageViews[i] = ImageView({
            .logicalDevice = m_spec.logicalDevice,
            .swapchain = this,
            .image = &m_images[i],
        });
    }
}

void Swapchain::recreate(std::vector<Framebuffer>& framebuffers, const RenderPass& renderPass) {
    CHECK(framebuffers.size() == m_imageViews.size());

    // query
    auto swapchainSupportDetails = vulkan::SwapchainSupportDetails::query(
        m_spec.physicalDevice->handle<VkPhysicalDevice>(), m_spec.surface->handle<VkSurfaceKHR>());
    m_extent2D = swapchainSupportDetails.optimalExtent(m_spec.window->handle<GLFWwindow*>());

    // if extent == 0 -> we're minimized -> wait idle until maximized
    while (m_extent2D.x == 0 || m_extent2D.y == 0) {
        glfwWaitEvents();
        swapchainSupportDetails = vulkan::SwapchainSupportDetails::query(
            m_spec.physicalDevice->handle<VkPhysicalDevice>(), m_spec.surface->handle<VkSurfaceKHR>());
        m_extent2D = swapchainSupportDetails.optimalExtent(m_spec.window->handle<GLFWwindow*>());
    }
    
    uint32 queueFamilyIndices[] = {
        m_spec.logicalDevice->graphicsQueue().index(),
        m_spec.logicalDevice->presentationQueue().index(),
    };
    bool sameQueueFamily = queueFamilyIndices[0] == queueFamilyIndices[1];

    // store old
    vk::SwapchainKHR oldSwapchain = as<vk::SwapchainKHR>();

    vk::SwapchainCreateInfoKHR swapchainCreateInfo = {
        .sType = vk::StructureType::eSwapchainCreateInfoKHR,
        .pNext = nullptr,
        .flags = {},
        .surface = m_spec.surface->handle<VkSurfaceKHR>(),
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
        .clipped = vk::True,
        .oldSwapchain = oldSwapchain,
    };

    setHandle(m_spec.logicalDevice->as<vk::Device>().createSwapchainKHR(swapchainCreateInfo));
    m_spec.logicalDevice->as<vk::Device>().destroySwapchainKHR(oldSwapchain);

    // restore
    m_images = Image::acquireImages({
        .logicalDevice = m_spec.logicalDevice,
        .swapchain = this,
    });

    for (usize i = 0; i < m_images.size(); i++) {
        m_imageViews[i] = ImageView({
            .logicalDevice = m_spec.logicalDevice,
            .swapchain = this,
            .image = &m_images[i],
        });

        framebuffers[i] = Framebuffer({
            .logicalDevice = m_spec.logicalDevice,
            .swapchain = this,
            .imageView = &m_imageViews[i],
            .renderPass = &renderPass,
        });
    }
}

Swapchain::~Swapchain() {
    if (validHandle()) {
        m_spec.logicalDevice->as<vk::Device>().destroySwapchainKHR(as<vk::SwapchainKHR>());
    }
}

} // namespace R3

#endif // R3_VULKAN
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file vulkan-Swapchain.hxx
/// @copyright GNU Public License
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#if R3_VULKAN

#include "render/Window.hpp"
#include "vulkan-fwd.hxx"
#include "vulkan-VulkanObject.hxx"
#include <api/Construct.hpp>
#include <api/Result.hpp>
#include <api/Types.hpp>
#include <span>
#include <tuple>
#include <vector>
#include <vulkan/vulkan.h>

namespace R3::vulkan {

/// @brief Swapchain details for checking suitable support.
struct SwapchainSupportDetails {
private:
    DEFAULT_CONSTRUCT(SwapchainSupportDetails);

public:
    /// @brief Checks is swapchain support at least one format and present mode.
    /// @return Validity
    constexpr bool valid() const { return !surfaceFormats.empty() && !presentModes.empty(); };

    /// @brief Get Swapchain details for PhsycialDevice and Surface.
    /// @param physicalDevice
    /// @param surface
    /// @return Swapchain Support Details | InitializationFailure
    /// @{
    static Result<SwapchainSupportDetails> query(const PhysicalDevice physicalDevice, const Surface& surface);
    static Result<SwapchainSupportDetails> query(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface);
    /// @}

    /// @brief Get optimal format channel and color space, preferably SRGB.
    /// @return Format, ColorSpace
    std::tuple<VkFormat, VkColorSpaceKHR> optimalSurfaceFormat() const;

    /// @brief Get optimal present mode for image ordering e.g. FIFO, MAILBOX.
    /// @return Present mode
    VkPresentModeKHR optimalPresentMode() const;

    /// @brief Get optimal extent, this is always equal to the window resolution.
    /// @param window OS Window
    /// @return Optimal Extent
    VkExtent2D optimalExtent(Window& window) const;

public:
    VkSurfaceCapabilitiesKHR capabilities = {};     ///< Swapchain capabilities
    std::vector<VkSurfaceFormatKHR> surfaceFormats; ///< all swapchain formats
    std::vector<VkPresentModeKHR> presentModes;     ///< all swapchain modes
};

/// @brief Vulkan Swapchain Specification
struct SwapchainSpecification {
    const Surface& surface;               ///< Valid Surface
    const PhysicalDevice& physicalDevice; ///< Valid PhysicalDevice
    const LogicalDevice& device;          ///< Valid LogicalDevice
    Window& window;                       ///< Valid Window
};

/// @brief Vulkan Swapchain Recreation Specification
struct SwapchainRecreationSpecification {
    Window& window;                         ///< Valid Window
    const Surface& surface;                 ///< Valid Surface
    const PhysicalDevice& physicalDevice;   ///< Valid PhysicalDevice
    const LogicalDevice& device;            ///< Valid LogicalDevice
    std::vector<Framebuffer>& framebuffers; ///< Framebuffers to be recreated
    ColorBuffer& colorBuffer;               ///< ColorBuffer to recreate
    DepthBuffer& depthBuffer;               ///< DepthBuffer to recreate
    const RenderPass& renderPass;           ///< RenderPass to reference
};

/// @brief Vulkan Swapchain RAII wrapper
/// Vulkan Swapchain is a queue of Images waiting to be presented to the screen, must be recreated on Window resize.
/// https://registry.khronos.org/vulkan/specs/1.3-extensions/man/html/VkSwapchainKHR.html
/// @note VK_KHR_swapchain device extension must be present
class Swapchain : public VulkanObject<VkSwapchainKHR> {
public:
    DEFAULT_CONSTRUCT(Swapchain);
    NO_COPY(Swapchain);
    DEFAULT_MOVE(Swapchain);

    /// @brief Creates Swapchain using optimal settings from Swapchain Details
    /// @param spec
    /// @return Swapchain | InitializationFailure
    static Result<Swapchain> create(const SwapchainSpecification& spec);

    /// @brief Destroys Swapchain and image views
    /// @note Swapchain images are device internal and must NOT be destroyed
    ~Swapchain();

    /// @brief Recreates swapchain using old Swapchain
    /// Also recreates ColorBuffer, DepthBuffer, and Framebuffers. If the new Swapchain fails to create the old
    /// Swapchain is preserved.
    /// @param spec
    /// @return void | InitializationFailure
    Result<void> recreate(const SwapchainRecreationSpecification& spec);

    /// @return Current surface format
    constexpr VkFormat surfaceFormat() const { return m_surfaceFormat; }

    /// @return Current color space
    constexpr VkColorSpaceKHR colorSpace() const { return m_colorSpace; }

    /// @return Current present mode
    constexpr VkPresentModeKHR presentMode() const { return m_presentMode; }

    /// @return Current extent
    constexpr VkExtent2D extent() const { return m_extent; }

    /// @return Current swapchain images
    constexpr std::span<const VkImage> images() const { return m_images; }

    /// @return Current swapchain image views
    constexpr std::span<const VkImageView> imageViews() const { return m_imageViews; }

private:
    VkDevice m_device = VK_NULL_HANDLE;

    VkFormat m_surfaceFormat       = VK_FORMAT_UNDEFINED;
    VkColorSpaceKHR m_colorSpace   = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
    VkPresentModeKHR m_presentMode = VK_PRESENT_MODE_FIFO_KHR;
    VkExtent2D m_extent            = {};
    uint32 m_imageCount            = 0;

    std::vector<VkImage> m_images;
    std::vector<VkImageView> m_imageViews;
};

} // namespace R3::vulkan

#endif // R3_VULKAN

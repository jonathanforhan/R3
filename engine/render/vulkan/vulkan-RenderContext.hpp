#pragma once

#include <cstdint>
#include <span>
#include <vector>
#include <VkBootstrap.h>
#include <vulkan/vulkan.h>
#include "engine/api/Api.hpp"
#include "engine/api/Class.hpp"
#include "engine/api/Types.hpp"
#include "engine/render/RenderContext.hpp"
#include "engine/render/Window.hpp"
#include "vulkan-CommandBuffer.hpp"
#include "vulkan-DescriptorSet.hpp"
#include "vulkan-Handle.hpp"

#ifdef R3_DEBUG
#define R3_VALIDATION_LAYERS_ENABLED 1
#else
#define R3_VALIDATION_LAYERS_ENABLED 0
#endif

namespace R3::vulkan {

/// @brief RenderContext holds vulkan resources:
///   - Instance
///   - Surface
///   - Device
///   - PhysicalDevice
class R3_API RenderContext : public IRenderContext {
public:
    R3_CTOR_DEFAULT(RenderContext);
    R3_COPY_DELETE(RenderContext);
    R3_MOVE_DEFAULT(RenderContext);

    /// @brief Create new RenderContext
    /// @param window Must be a valid Window for surface creation
    explicit RenderContext(Window& window);

    /// Destroys the RenderContext
    virtual ~RenderContext() noexcept override;

    /// Returns the maximum number of frames that can be processed concurrently (in flight).
    constexpr uint32 maxFramesInFlight() const noexcept { return 3; }
    /// Returns the maximum number of shader texture sampler bindings.
    constexpr uint32 maxTextureSamplerBindings() const noexcept { return 1024; }
    /// Increment the current frame index or reset when equal to maxFramesInFlight()
    void advanceFrame() noexcept { m_currentFrame = (m_currentFrame + 1) % maxFramesInFlight(); }

    /// Wait on the host for the completion of outstanding queue operations for all queues in this context
    void waitIdle();
    /// Wait for the current frame to finish and reset its fence
    void waitForCurrentFrame();

    /// Get the index of memory type which satisfies ```typeFilter``` and ```properties```
    uint32 queryDeviceMemoryTypeIndex(uint32 typeFilter, VkMemoryPropertyFlags properties) const;

    /// Get MSAA sample count supported by the physical device
    VkSampleCountFlagBits queryMaxUsableSampleCount() const noexcept;
    /// Get physical device depth format
    VkFormat queryDepthFormat() const noexcept;
    /// Get physical device supported format chosen from a list of formats
    VkFormat querySupportedFormat(std::span<const VkFormat> formats,
                                  VkImageTiling tiling,
                                  VkFormatFeatureFlags features) const noexcept;
    VkFormat swapchainFormat() const noexcept { return m_swapchainFormat; }
    void setSwapchainFormat(VkFormat format) noexcept { m_swapchainFormat = format; }

    /// VkInstance getter
    VkInstance instance() const noexcept { return m_instance; }
    /// VkSurfaceKHR getter
    VkSurfaceKHR surface() const noexcept { return m_surface; }
    /// VkPhysicalDevice getter
    VkPhysicalDevice physicalDevice() const noexcept { return m_physicalDevice; }
    /// VkDevice getter
    VkDevice device() const noexcept { return m_device; }

    /// Graphics VkQueue getter
    VkQueue graphicsQueue() const noexcept { return m_graphicsQueue; }
    /// Present VkQueue getter
    VkQueue presentQueue() const noexcept { return m_presentQueue; }
    /// Compute VkQueue getter
    VkQueue computeQueue() const noexcept { return m_computeQueue; }

    /// Graphics queue family index getter
    uint32 graphicsQueueIndex() const noexcept { return m_graphicsQueueIndex; }
    /// Present queue family index getter
    uint32 presentQueueIndex() const noexcept { return m_presentQueueIndex; }
    /// Compute queue family index getter
    uint32 computeQueueIndex() const noexcept { return m_computeQueueIndex; }

    /// Retrieves the graphics queue command buffer for the current frame.
    CommandBuffer& graphicsCommandBuffer() noexcept { return m_graphicsQueueCmds[m_currentFrame]; }
    /// Retrieves the compute queue command buffer for the current frame.
    CommandBuffer& computeCommandBuffer() noexcept { return m_computeQueueCmds[m_currentFrame]; }

    /// Get the image available semaphore for the current frame
    VkSemaphore& currentImageAvailableSemaphore() noexcept { return m_imageAvailableSemaphores[m_currentFrame]; }
    /// Get the fence for the current frame
    VkFence& currentFence() noexcept { return m_inFlightFences[m_currentFrame]; }
    /// Get the current frame index
    uint32 currentFrameIndex() const noexcept { return m_currentFrame; };
    /// Get the render finished semaphore for the specified swapchain image index
    VkSemaphore& renderFinishedSemaphore(usize imageIndex) noexcept { return m_renderFinishedSemaphores[imageIndex]; }

    /// Get global descriptor set layout
    VkDescriptorSetLayout descriptorLayout() const noexcept { return m_descriptorSetLayout; }
    /// Get global descriptor sets
    DescriptorSet& descriptorSet(usize index) noexcept { return m_descriptorSets[index]; }
    const DescriptorSet& descriptorSet(usize index) const noexcept { return m_descriptorSets[index]; }

private:
    static vkb::Instance createInstance();
    static VkSurfaceKHR createSurface(Window& window, VkInstance instance);
    static vkb::PhysicalDevice selectPhysicalDevice(const vkb::Instance& instance, VkSurfaceKHR surface);
    static vkb::Device createLogicalDevice(const vkb::PhysicalDevice& physicalDevice);
    static void setupQueue(const vkb::Device& device, vkb::QueueType queueType, VkQueue& queue, uint32& index);
    void createCommandPools();
    void createSyncObjects();
    void createDescriptorSets();

private:
    Handle<VkInstance> m_instance;
    Handle<VkDebugUtilsMessengerEXT> m_debug;
    Handle<VkSurfaceKHR> m_surface;
    Handle<VkPhysicalDevice> m_physicalDevice;
    Handle<VkDevice> m_device;
    Handle<VkQueue> m_graphicsQueue;
    Handle<VkQueue> m_presentQueue;
    Handle<VkQueue> m_computeQueue;
    uint32 m_graphicsQueueIndex = UINT32_MAX;
    uint32 m_presentQueueIndex  = UINT32_MAX;
    uint32 m_computeQueueIndex  = UINT32_MAX;
    std::vector<CommandBuffer> m_graphicsQueueCmds;
    std::vector<CommandBuffer> m_computeQueueCmds;
    std::vector<VkSemaphore> m_imageAvailableSemaphores; // one per frame in flight
    std::vector<VkFence> m_inFlightFences;               // one per frame in flight
    std::vector<VkSemaphore> m_renderFinishedSemaphores; // one per swapchain image
    uint32 m_currentFrame = 0;
    Handle<VkDescriptorSetLayout> m_descriptorSetLayout;
    std::vector<DescriptorSet> m_descriptorSets;
    VkFormat m_swapchainFormat = {};
};

} // namespace R3::vulkan

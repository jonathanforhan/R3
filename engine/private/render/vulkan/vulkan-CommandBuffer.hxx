#pragma once

#if R3_VULKAN

#include "vulkan-Framebuffer.hxx"
#include "vulkan-fwd.hxx"
#include "vulkan-GraphicsPipeline.hxx"
#include "vulkan-LogicalDevice.hxx"
#include "vulkan-RenderPass.hxx"
#include "vulkan-Semaphore.hxx"
#include "vulkan-Swapchain.hxx"
#include "vulkan-VulkanObject.hxx"
#include <api/Assert.hpp>
#include <api/Types.hpp>
#include <span>
#include <vulkan/vulkan.h>

namespace R3::vulkan {

struct CommandBufferSubmitSpecification {
    const LogicalDevice& device;
    VkPipelineStageFlags waitStages;
    std::span<const VkSemaphore> waitSemaphores;
    std::span<VkSemaphore> signalSemaphores;
    VkFence fence = VK_NULL_HANDLE; // optional
};

struct CommandBufferPresentSpecification {
    const LogicalDevice& device;
    const Swapchain& swapchain;
    std::span<const VkSemaphore> waitSemaphores;
    uint32 currentImage;
};

// scope unique buffer friends w/ cmd pool
class CommandBuffer : public VulkanObject<VkCommandBuffer> {
public:
    DEFAULT_COPY(CommandBuffer);
    DEFAULT_MOVE(CommandBuffer);

    CommandBuffer(VkCommandBuffer commandBuffer);

    void reset();

    void begin(VkCommandBufferUsageFlags flags = 0);

    void end();

    void beginRenderPass(const RenderPass& renderPass, const Framebuffer& framebuffer, VkExtent2D extent);

    void endRenderPass();

    void bindPipeline(const GraphicsPipeline& pipeline);

    void setViewport(const Swapchain& swapchain);

    void setScissor(const Swapchain& swapchain);

    // void bindVertexBuffer(const VertexBuffer& pipeline);

    // void bindIndexBuffer(const IndexBuffer& pipeline);

    void bindDescriptorSet(VkPipelineLayout layout, VkDescriptorSet descriptorSet);

    // void pushConstants(...);

    void submit(const CommandBufferSubmitSpecification& spec);

    VkResult present(const CommandBufferPresentSpecification& spec);
};

inline CommandBuffer::CommandBuffer(VkCommandBuffer commandBuffer) {
    m_handle = commandBuffer;
}

inline void CommandBuffer::reset() {
    vkResetCommandBuffer(m_handle, 0);
}

inline void CommandBuffer::begin(VkCommandBufferUsageFlags flags) {
    const VkCommandBufferBeginInfo commandBufferBeginInfo = {
        .sType            = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .pNext            = nullptr,
        .flags            = flags,
        .pInheritanceInfo = nullptr,
    };

    VkResult result = vkBeginCommandBuffer(m_handle, &commandBufferBeginInfo);
    ENSURE(result == VK_SUCCESS);
}

inline void CommandBuffer::end() {
    vkEndCommandBuffer(m_handle);
}

inline void CommandBuffer::beginRenderPass(const RenderPass& renderPass,
                                           const Framebuffer& framebuffer,
                                           VkExtent2D extent) {
    static constexpr VkClearValue clearColor = {
        .color = {0.0f, 0.0f, 0.0f, 1.0f},
    };

    static constexpr VkClearValue clearDepthStencil = {
        .depthStencil = {.depth = 1.0f, .stencil = 0},
    };

    static constexpr VkClearValue clearValues[] = {
        clearColor,
        clearDepthStencil,
    };

    const VkRenderPassBeginInfo renderPassBeginInfo = {
        .sType       = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
        .pNext       = nullptr,
        .renderPass  = renderPass.vk(),
        .framebuffer = framebuffer.vk(),
        .renderArea =
            {
                .offset = {},
                .extent = extent,
            },
        .clearValueCount = static_cast<uint32>(std::size(clearValues)),
        .pClearValues    = clearValues,
    };

    vkCmdBeginRenderPass(m_handle, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
}

inline void CommandBuffer::endRenderPass() {
    vkCmdEndRenderPass(m_handle);
}

inline void CommandBuffer::bindPipeline(const GraphicsPipeline& pipeline) {
    vkCmdBindPipeline(m_handle, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline.vk());
}

inline void CommandBuffer::setViewport(const Swapchain& swapchain) {
    const VkViewport viewport = {
        .x        = 0.0f,
        .y        = static_cast<float>(swapchain.extent().height),
        .width    = static_cast<float>(swapchain.extent().width),
        .height   = -static_cast<float>(swapchain.extent().height),
        .minDepth = 0.0f,
        .maxDepth = 1.0f,
    };
    vkCmdSetViewport(m_handle, 0, 1, &viewport);
}

inline void CommandBuffer::setScissor(const Swapchain& swapchain) {
    const VkRect2D scissor = {
        .offset = {},
        .extent = swapchain.extent(),
    };
    vkCmdSetScissor(m_handle, 0, 1, &scissor);
}

inline void CommandBuffer::bindDescriptorSet(VkPipelineLayout layout, VkDescriptorSet descriptorSet) {
    vkCmdBindDescriptorSets(m_handle, VK_PIPELINE_BIND_POINT_GRAPHICS, layout, 0, 1, &descriptorSet, 0, nullptr);
}

inline void CommandBuffer::submit(const CommandBufferSubmitSpecification& spec) {
    const VkSubmitInfo submitInfo = {
        .sType                = VK_STRUCTURE_TYPE_SUBMIT_INFO,
        .pNext                = nullptr,
        .waitSemaphoreCount   = static_cast<uint32>(spec.waitSemaphores.size()),
        .pWaitSemaphores      = spec.waitSemaphores.data(),
        .pWaitDstStageMask    = &spec.waitStages,
        .commandBufferCount   = 1,
        .pCommandBuffers      = &m_handle,
        .signalSemaphoreCount = static_cast<uint32>(spec.signalSemaphores.size()),
        .pSignalSemaphores    = spec.signalSemaphores.data(),
    };

    spec.device.graphicsQueue().lock();
    (void)vkQueueSubmit(spec.device.graphicsQueue().vk(), 1, &submitInfo, spec.fence);
    spec.device.graphicsQueue().unlock();
}

inline VkResult CommandBuffer::present(const CommandBufferPresentSpecification& spec) {
    VkSwapchainKHR swapchain = spec.swapchain.vk();

    const VkPresentInfoKHR presentInfo = {
        .sType              = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
        .pNext              = nullptr,
        .waitSemaphoreCount = static_cast<uint32>(spec.waitSemaphores.size()),
        .pWaitSemaphores    = spec.waitSemaphores.data(),
        .swapchainCount     = 1,
        .pSwapchains        = &swapchain,
        .pImageIndices      = &spec.currentImage,
        .pResults           = nullptr,
    };

    return vkQueuePresentKHR(spec.device.presentationQueue().vk(), &presentInfo);
}

} // namespace R3::vulkan

#endif // R3_VULKAN

#if R3_VULKAN

#include "render/CommandBuffer.hpp"

#include <vulkan/vulkan.h>
#include "api/Check.hpp"
#include "api/Ensure.hpp"
#include "render/CommandPool.hpp"
#include "render/Framebuffer.hpp"
#include "render/GraphicsPipeline.hpp"
#include "render/LogicalDevice.hpp"
#include "render/RenderPass.hpp"
#include "render/Swapchain.hpp"
#include "render/VertexBuffer.hpp"

namespace R3 {

void CommandBuffer::create(const CommandBufferSpecification& spec) {
    CHECK(spec.logicalDevice != nullptr);
    CHECK(spec.swapchain != nullptr);
    CHECK(spec.commandPool != nullptr);
    m_spec = spec;

    VkCommandBufferAllocateInfo commandBufferAllocateInfo = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .pNext = nullptr,
        .commandPool = m_spec.commandPool->handle<VkCommandPool>(),
        .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        .commandBufferCount = 1,
    };

    ENSURE(vkAllocateCommandBuffers(m_spec.logicalDevice->handle<VkDevice>(),
                                    &commandBufferAllocateInfo,
                                    handlePtr<VkCommandBuffer*>()) == VK_SUCCESS);
}

void CommandBuffer::destroy() {
    vkFreeCommandBuffers(m_spec.logicalDevice->handle<VkDevice>(),
                         m_spec.commandPool->handle<VkCommandPool>(),
                         1,
                         handlePtr<VkCommandBuffer*>());
}

void CommandBuffer::resetCommandBuffer() {
    vkResetCommandBuffer(handle<VkCommandBuffer>(), 0U);
}

void CommandBuffer::beginCommandBuffer() {
    VkCommandBufferBeginInfo commandBufferBeginInfo = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .pNext = nullptr,
        .flags = 0U,
        .pInheritanceInfo = nullptr,
    };

    VkResult result = vkBeginCommandBuffer(handle<VkCommandBuffer>(), &commandBufferBeginInfo);
    CHECK(result == VK_SUCCESS);
}

void CommandBuffer::endCommandBuffer() {
    VkResult result = vkEndCommandBuffer(handle<VkCommandBuffer>());
    CHECK(result == VK_SUCCESS);
}

void CommandBuffer::beginRenderPass(const RenderPass& renderPass, const Framebuffer& framebuffer) {
    VkClearValue clearColor = {{{0.0f, 0.0f, 0.0f, 1.0f}}};

    VkRenderPassBeginInfo renderPassBeginInfo = {
        .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
        .pNext = nullptr,
        .renderPass = renderPass.handle<VkRenderPass>(),
        .framebuffer = framebuffer.handle<VkFramebuffer>(),
        .renderArea =
            {
                .offset = {0, 0},
                .extent = {m_spec.swapchain->extent().x, m_spec.swapchain->extent().y},
            },
        .clearValueCount = 1,
        .pClearValues = &clearColor,
    };

    vkCmdBeginRenderPass(handle<VkCommandBuffer>(), &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
}

void CommandBuffer::endRenderPass() {
    vkCmdEndRenderPass(handle<VkCommandBuffer>());
}

void CommandBuffer::bindPipeline(const GraphicsPipeline& graphicsPipeline) {
    vkCmdBindPipeline(
        handle<VkCommandBuffer>(), VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline.handle<VkPipeline>());

    VkViewport viewport = {
        .x = 0.0f,
        .y = 0.0f,
        .width = static_cast<float>(m_spec.swapchain->extent().x),
        .height = static_cast<float>(m_spec.swapchain->extent().y),
        .minDepth = 0.0f,
        .maxDepth = 1.0f,
    };
    vkCmdSetViewport(handle<VkCommandBuffer>(), 0, 1, &viewport);

    VkRect2D scissor = {
        .offset = {0, 0},
        .extent = {m_spec.swapchain->extent().x, m_spec.swapchain->extent().y},
    };
    vkCmdSetScissor(handle<VkCommandBuffer>(), 0, 1, &scissor);
}

void CommandBuffer::bindVertexBuffers(const std::vector<VertexBuffer>& vertexBuffers) {
    std::vector<VkBuffer> buffers(vertexBuffers.size());
    for (uint32 i = 0; auto& vertexBuffer : vertexBuffers) {
        buffers[i] = vertexBuffer.handle<VkBuffer>();
        i++;
    }
    VkDeviceSize offsets[] = {0};
    vkCmdBindVertexBuffers(handle<VkCommandBuffer>(), 0, 1, buffers.data(), offsets);
}

} // namespace R3

#endif // R3_VULKAN
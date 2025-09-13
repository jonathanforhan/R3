#include "vulkan-RenderPass.hpp"

#include <optional>
#include <vector>
#include <vulkan/vulkan.h>
#include "api/Assert.hpp"
#include "api/Types.hpp"
#include "vulkan-CommandBuffer.hpp"

namespace R3::vulkan {

void RenderPass::execute(CommandBuffer& cmd) {
    R3_ASSERT(m_pipeline);

    if (!m_barriers.empty() || !m_imageBarriers.empty()) {
        cmd.pipelineBarrier({
            .sType                   = VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
            .memoryBarrierCount      = static_cast<uint32>(m_barriers.size()),
            .pMemoryBarriers         = m_barriers.data(),
            .imageMemoryBarrierCount = static_cast<uint32>(m_imageBarriers.size()),
            .pImageMemoryBarriers    = m_imageBarriers.data(),
        });
    }

    // Set up rendering attachments if this pass renders
    if (!m_colorAttachments.empty() || m_depthAttachment.has_value()) {
        cmd.beginRendering({
            .sType                = VK_STRUCTURE_TYPE_RENDERING_INFO,
            .renderArea           = m_renderArea,
            .layerCount           = 1,
            .colorAttachmentCount = static_cast<uint32>(m_colorAttachments.size()),
            .pColorAttachments    = m_colorAttachments.data(),
            .pDepthAttachment     = m_depthAttachment ? &*m_depthAttachment : nullptr,
        });
        render(cmd);
        cmd.endRendering();
    } else {
        // Compute/transfer pass - no rendering
        render(cmd);
    }
}

void RenderPass::setDynamicPipelineStates(CommandBuffer& cmd) {
    cmd.setScissor(m_renderArea);
    cmd.setViewport({
        .x        = 0.0f,
        .y        = static_cast<float>(m_renderArea.extent.height),
        .width    = static_cast<float>(m_renderArea.extent.width),
        .height   = -static_cast<float>(m_renderArea.extent.height),
        .minDepth = 0.0f,
        .maxDepth = 1.0f,
    });
    cmd.setCullMode(VK_CULL_MODE_BACK_BIT);
    cmd.setLineWidth(1.0f);
    cmd.setFrontFace(VK_FRONT_FACE_COUNTER_CLOCKWISE);
    cmd.setDepthTestEnable(true);
}

} // namespace R3::vulkan
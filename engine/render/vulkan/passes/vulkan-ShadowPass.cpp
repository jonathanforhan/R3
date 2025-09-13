#include "vulkan-ShadowPass.hpp"

#include "../vulkan-CommandBuffer.hpp"
#include "../vulkan-GraphicsPipeline.hpp"
#include "../vulkan-RenderPass.hpp"
#include "api/Types.hpp"
#include "components/MeshComponent.hpp"
#include "components/TransformComponent.hpp"
#include "core/Engine.hpp"
#include "core/World.hpp"
#include "render/Buffer.hpp"
#include "render/ShaderObjects.hpp"

namespace R3::vulkan {

void ShadowPass::setDynamicPipelineStates(CommandBuffer& cmd) {
    RenderPass::setDynamicPipelineStates(cmd);
    cmd.setCullMode(VK_CULL_MODE_FRONT_BIT);
    cmd.setViewport({
        .x        = 0.0f,
        .y        = 0.0f,
        .width    = static_cast<float>(m_renderArea.extent.width),
        .height   = static_cast<float>(m_renderArea.extent.height),
        .minDepth = 0.0f,
        .maxDepth = 1.0f,
    });
}

void ShadowPass::render(CommandBuffer& cmd) {
    cmd.bindGraphicsPipeline(m_pipeline->pipeline());
    setDynamicPipelineStates(cmd);

    cmd.bindDescriptorSets({
        .sType              = VK_STRUCTURE_TYPE_BIND_DESCRIPTOR_SETS_INFO,
        .stageFlags         = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
        .layout             = m_pipeline->layout(),
        .firstSet           = 0,
        .descriptorSetCount = 1,
        .pDescriptorSets    = &m_descriptorSet,
    });

    GWorld()->registry().view<MeshComponent, TransformComponent>().each(
        [&](const MeshComponent& mesh, const TransformComponent& trans) {
            const VertexPushConstantsShadow vertPush = {
                .model         = trans.transform(),
                .lightViewProj = m_lightSpaceMatrix,
            };
            cmd.pushConstants({
                .sType      = VK_STRUCTURE_TYPE_PUSH_CONSTANTS_INFO,
                .layout     = m_pipeline->layout(),
                .stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
                .offset     = 0,
                .size       = sizeof(VertexPushConstantsShadow),
                .pValues    = &vertPush,
            });

            const VkBuffer vboIndices[]  = {mesh.vertexBufferIndex->bufferHandle()};
            const VkDeviceSize offsets[] = {0};
            const VkBuffer iboIndex      = mesh.indexBufferIndex->bufferHandle();
            cmd.bindVertexBuffers(0, vboIndices, offsets);
            cmd.bindIndexBuffer(iboIndex, 0, VK_INDEX_TYPE_UINT32);
            cmd.drawIndexed(static_cast<uint32>(mesh.indexCount));
        });
}

} // namespace R3::vulkan

#include "vulkan-EditorPass.hpp"

#include "../vulkan-CommandBuffer.hpp"
#include "../vulkan-GraphicsPipeline.hpp"
#include "../vulkan-RenderPass.hpp"
#include "api/Assert.hpp"
#include "api/Types.hpp"
#include "components/MaterialComponent.hpp"
#include "components/MeshComponent.hpp"
#include "components/TransformComponent.hpp"
#include "core/Engine.hpp"
#include "core/Entity.hpp"
#include "core/World.hpp"
#include "render/Buffer.hpp"
#include "render/ShaderObjects.hpp"

namespace R3::vulkan {

void EditorPass::render(CommandBuffer& cmd) {
    R3_ASSERT(m_pipeline, "Pipeline not set for EditorPass");

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

    GWorld()->registry().view<MeshComponent, MaterialComponent, TransformComponent>().each(
        [&](Entity entity, const MeshComponent& mesh, const MaterialComponent& mat, const TransformComponent& trans) {
            const EditorVertexPushConstants vertPushConstants = {
                .model = trans.transform(),
            };
            cmd.pushConstants({
                .sType      = VK_STRUCTURE_TYPE_PUSH_CONSTANTS_INFO,
                .layout     = m_pipeline->layout(),
                .stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
                .offset     = 0,
                .size       = sizeof(EditorVertexPushConstants),
                .pValues    = &vertPushConstants,
            });

            const EditorFragmentPushConstants fragPushConstants = {
                .entityID = (uint32)entity,
            };
            cmd.pushConstants({
                .sType      = VK_STRUCTURE_TYPE_PUSH_CONSTANTS_INFO,
                .layout     = m_pipeline->layout(),
                .stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT,
                .offset     = sizeof(EditorVertexPushConstants),
                .size       = sizeof(EditorFragmentPushConstants),
                .pValues    = &fragPushConstants,
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
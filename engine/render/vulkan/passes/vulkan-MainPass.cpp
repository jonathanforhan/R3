#include "vulkan-MainPass.hpp"

#include "../vulkan-CommandBuffer.hpp"
#include "../vulkan-GraphicsPipeline.hpp"
#include "../vulkan-RenderPass.hpp"
#include "api/Assert.hpp"
#include "api/Types.hpp"
#include "components/MaterialComponent.hpp"
#include "components/MeshComponent.hpp"
#include "components/TransformComponent.hpp"
#include "core/Camera.hpp"
#include "core/Engine.hpp"
#include "core/Entity.hpp"
#include "core/World.hpp"
#include "render/Buffer.hpp"
#include "render/ShaderObjects.hpp"

namespace R3::vulkan {

void MainPass::render(CommandBuffer& cmd) {
    if (m_cubemapPipeline && m_cubemapTextureSlot != 0xFFFFFFFF) {
        renderCubemap(cmd);
    }
    renderScene(cmd);
}

void MainPass::renderCubemap(CommandBuffer& cmd) {
    cmd.bindGraphicsPipeline(m_cubemapPipeline->pipeline());
    setDynamicPipelineStates(cmd);
    cmd.setDepthTestEnable(false); // Disable depth for skybox

    cmd.bindDescriptorSets({
        .sType              = VK_STRUCTURE_TYPE_BIND_DESCRIPTOR_SETS_INFO,
        .stageFlags         = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
        .layout             = m_cubemapPipeline->layout(),
        .firstSet           = 0,
        .descriptorSetCount = 1,
        .pDescriptorSets    = &m_descriptorSet,
    });

    // Push Constants
    const FragmentPushConstantsCubemap fragPushConstants = {.iCubemap = m_cubemapTextureSlot};
    cmd.pushConstants({
        .sType      = VK_STRUCTURE_TYPE_PUSH_CONSTANTS_INFO,
        .layout     = m_cubemapPipeline->layout(),
        .stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT,
        .offset     = 0,
        .size       = sizeof(FragmentPushConstantsCubemap),
        .pValues    = &fragPushConstants,
    });

    cmd.draw(36, 1, 0, 0);
}

void MainPass::renderScene(CommandBuffer& cmd) {
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
            const VertexPushConstants vertPushConstants = {
                .model = trans.transform(),
            };
            cmd.pushConstants({
                .sType      = VK_STRUCTURE_TYPE_PUSH_CONSTANTS_INFO,
                .layout     = m_pipeline->layout(),
                .stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
                .offset     = 0,
                .size       = sizeof(VertexPushConstants),
                .pValues    = &vertPushConstants,
            });

            const FragmentPushConstants fragPushConstants = {
                .viewPosition       = GWorld()->camera().position(),
                .numLights          = m_lightCount,
                .iAlbedo            = mat.iAlbedo,
                .iMetallicRoughness = mat.iMetallicRoughness,
                .iNormal            = mat.iNormal,
                .iAmbientOcclusion  = mat.iAmbientOcclusion,
                .iEmissive          = mat.iEmissive,
                .entityID           = (uint32)entity,
            };
            cmd.pushConstants({
                .sType      = VK_STRUCTURE_TYPE_PUSH_CONSTANTS_INFO,
                .layout     = m_pipeline->layout(),
                .stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT,
                .offset     = sizeof(VertexPushConstants),
                .size       = sizeof(FragmentPushConstants),
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
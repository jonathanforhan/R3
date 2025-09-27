#pragma once

#include <functional>
#include <initializer_list>
#include <vulkan/vulkan.h>
#include "engine/api/Api.hpp"
#include "engine/api/Class.hpp"
#include "engine/api/Types.hpp"
#include "engine/render/RenderFwd.hpp"
#include "vulkan-Fwd.hpp"
#include "vulkan-Handle.hpp"

namespace R3::vulkan {

class R3_API GraphicsPipeline {
public:
    R3_CTOR_DEFAULT(GraphicsPipeline);
    R3_COPY_DELETE(GraphicsPipeline);
    R3_MOVE_DEFAULT(GraphicsPipeline);

    GraphicsPipeline(RenderContext& ctx,
                     Shader& vertexShader,
                     Shader& fragmentShader,
                     uint32 msaaSamples,
                     std::initializer_list<VkFormat> colorFormats,
                     std::initializer_list<VkPipelineColorBlendAttachmentState> colorBlends,
                     std::initializer_list<VkDescriptorSetLayout> layouts);

    ~GraphicsPipeline() noexcept;

    VkPipeline pipeline() const noexcept { return m_pipeline; }

    VkPipelineLayout layout() const noexcept { return m_pipelineLayout; }

private:
    Handle<VkDevice> m_device;
    Handle<VkPipeline> m_pipeline;
    Handle<VkPipelineLayout> m_pipelineLayout;
};

} // namespace R3::vulkan
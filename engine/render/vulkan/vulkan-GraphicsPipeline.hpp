#pragma once

#include <functional>
#include <span>
#include <vector>
#include <vulkan/vulkan.h>
#include "engine/api/Api.hpp"
#include "engine/api/Class.hpp"
#include "vulkan-Fwd.hpp"
#include "vulkan-Handle.hpp"

namespace R3::vulkan {

class R3_API GraphicsPipeline {
public:
    R3_CTOR_DEFAULT(GraphicsPipeline);
    R3_COPY_DELETE(GraphicsPipeline);
    R3_MOVE_DEFAULT(GraphicsPipeline);

    GraphicsPipeline(RenderContext& ctx,
                     std::vector<std::reference_wrapper<Shader>> shaders,
                     VkSampleCountFlagBits msaaSamples,
                     std::vector<VkFormat> colorFormats,
                     std::vector<VkDescriptorSetLayout> layouts,
                     std::vector<VkPushConstantRange> pushConstantRanges,
                     std::vector<VkVertexInputBindingDescription> vertexBindingDescription      = {},
                     std::vector<VkVertexInputAttributeDescription> vertexAttributeDescriptions = {});

    ~GraphicsPipeline() noexcept;

    VkPipeline pipeline() const noexcept { return m_pipeline; }

    VkPipelineLayout layout() const noexcept { return m_pipelineLayout; }

private:
    Handle<VkDevice> m_device;
    Handle<VkPipeline> m_pipeline;
    Handle<VkPipelineLayout> m_pipelineLayout;
};

} // namespace R3::vulkan
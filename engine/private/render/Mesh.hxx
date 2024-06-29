#pragma once

#if R3_VULKAN
#include "vulkan/vulkan-DescriptorPool.hxx"
#include "vulkan/vulkan-GraphicsPipeline.hxx"
#include "vulkan/vulkan-VertexBuffer.hxx"
#endif

#include "ShaderObjects.hxx"

namespace R3 {

class Mesh {
public:
#if R3_VULKAN
    vulkan::GraphicsPipeline graphicsPipeline;
    vulkan::DescriptorPool descriptorPool;
#endif
};

} // namespace R3
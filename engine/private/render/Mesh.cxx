#include "render/Mesh.hpp"

#include "core/Engine.hpp"

namespace R3 {

Mesh::Mesh(const MeshSpecification& spec) {
    m_vertexBuffer = VertexBuffer({
        .physicalDevice = spec.physicalDevice,
        .logicalDevice = spec.logicalDevice,
        .commandPool = spec.commandPool,
        .vertices = spec.vertices,
    });

    m_indexBuffer = IndexBuffer<uint32>({
        .physicalDevice = spec.physicalDevice,
        .logicalDevice = spec.logicalDevice,
        .commandPool = spec.commandPool,
        .indices = spec.indices,
    });

    m_material = Material(spec.materialSpecification);

    m_pipeline = GraphicsPipeline({
        .logicalDevice = Engine::renderer().logicalDevice(),
        .swapchain = Engine::renderer().swapchain(),
        .renderPass = Engine::renderer().renderPass(),
        .descriptorSetLayout = &m_material.descriptorPool().layout(),
        .vertexShaderPath = spec.vertexShaderPath,
        .fragmentShaderPath = spec.fragmentShaderPath,
    });
}

} // namespace R3

#if R3_VULKAN

#include "render/Renderer.hpp"
#include "detail/VulkanRenderer.hxx"

namespace R3 {

Renderer::Renderer()
    : m_vulkanRenderer(nullptr) {
    m_vulkanRenderer = new vulkan::VulkanRenderer;
}

void Renderer::enableCulling() {
}

void Renderer::enableDepthTest() {
}

void Renderer::enableStencilTest() {
}

void Renderer::enableMultisample() {
}

void Renderer::disableCulling() {
}

void Renderer::disableDepthTest() {
}

void Renderer::disableStencilTest() {
}

void Renderer::disableMultisample() {
}

void Renderer::depthFunction(DepthFunction func) {
}

void Renderer::depthMask(bool b) {
}

void Renderer::stencilFunction(StencilFunction func, int32 ref, uint8 mask) {
}

void Renderer::stencilMask(uint8 mask) {
}

void Renderer::stencilOperation(StencilOperation stencilFail, StencilOperation depthFail, StencilOperation pass) {
}

void Renderer::predraw() const {
}

void Renderer::drawElements(RenderPrimitive primitive, uint32 indiceCount) const {
}

void Renderer::drawArrays(RenderPrimitive primitive, uint32 vertexCount) const {
}

} // namespace R3

#endif // R3_VULKAN

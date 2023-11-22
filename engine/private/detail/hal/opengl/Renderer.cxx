#if R3_OPENGL

#include "core/Renderer.hpp"
#include <glad/glad.h>
#include "api/Log.hpp"

namespace R3 {

static constexpr GLenum renderPrimitiveToGlEnum(RenderPrimitive primitive) {
    switch (primitive) {
        case RenderPrimitive::Triangles:
            return GL_TRIANGLES;
        case RenderPrimitive::Lines:
            return GL_LINES;
        case RenderPrimitive::Points:
            return GL_POINTS;
        default:
            return NULL;
    }
}

static constexpr GLenum depthFunctionToGlDepthFunc(DepthFunction func) {
    switch (func) {
        case DepthFunction::Always:
            return GL_ALWAYS;
        case DepthFunction::Never:
            return GL_NEVER;
        case DepthFunction::Less:
            return GL_LESS;
        case DepthFunction::Equal:
            return GL_EQUAL;
        case DepthFunction::LessEqual:
            return GL_LEQUAL;
        case DepthFunction::Greater:
            return GL_GREATER;
        case DepthFunction::NotEqual:
            return GL_NOTEQUAL;
        case DepthFunction::GreaterEqual:
            return GL_GEQUAL;
    }
}

static constexpr GLenum stencilFunctionToGlStencilFunc(StencilFunction func) {
    switch (func) {
        case StencilFunction::Always:
            return GL_ALWAYS;
        case StencilFunction::Equal:
            return GL_EQUAL;
        case StencilFunction::NotEqual:
            return GL_NOTEQUAL;
        default:
            return NULL;
    }
}

static constexpr GLenum stencilOperationToGlStencilOp(StencilOperation op) {
    switch (op) {
        case StencilOperation::Keep:
            return GL_KEEP;
        case StencilOperation::Zero:
            return GL_ZERO;
        case StencilOperation::Replace:
            return GL_REPLACE;
        case StencilOperation::Increment:
            return GL_INCR;
        case StencilOperation::IncrementWrap:
            return GL_INCR_WRAP;
        case StencilOperation::Decrement:
            return GL_DECR;
        case StencilOperation::DecrementWrap:
            return GL_DECR_WRAP;
        case StencilOperation::Invert:
            return GL_INVERT;
        default:
            return NULL;
    }
}

Renderer::Renderer()
    : m_framebuffer(),
      m_renderbuffer() {
    m_framebuffer.attachRenderBuffer(m_renderbuffer);

    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_STENCIL_TEST);
    glEnable(GL_MULTISAMPLE);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    GLint maxImages;
    glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &maxImages);
    LOG(Verbose, "Texture Slots", maxImages);
}

void Renderer::enableCulling() {
    glEnable(GL_CULL_FACE);
}

void Renderer::enableDepthTest() {
    glEnable(GL_DEPTH_TEST);
}

void Renderer::enableStencilTest() {
    glEnable(GL_STENCIL_TEST);
}

void Renderer::enableMultisample() {
    glEnable(GL_MULTISAMPLE);
}

void Renderer::disableCulling() {
    glDisable(GL_CULL_FACE);
}

void Renderer::disableDepthTest() {
    glDisable(GL_DEPTH_TEST);
}

void Renderer::disableStencilTest() {
    glDisable(GL_STENCIL_TEST);
}

void Renderer::disableMultisample() {
    glDisable(GL_MULTISAMPLE);
}

void Renderer::depthFunction(DepthFunction func) {
    glDepthFunc(depthFunctionToGlDepthFunc(func));
}

void Renderer::depthMask(bool b) {
    glDepthMask(b);
}

void Renderer::stencilFunction(StencilFunction func, int32 ref, uint8 mask) {
    glStencilFunc(stencilFunctionToGlStencilFunc(func), ref, mask);
}

void Renderer::stencilMask(uint8 mask) {
    glStencilMask(mask);
}

void Renderer::stencilOperation(StencilOperation stencilFail, StencilOperation depthFail, StencilOperation pass) {
    GLenum sFail = stencilOperationToGlStencilOp(stencilFail);
    GLenum dpFail = stencilOperationToGlStencilOp(depthFail);
    GLenum allPass = stencilOperationToGlStencilOp(pass);
    glStencilOp(sFail, dpFail, allPass);
}

void Renderer::predraw() const {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    Framebuffer::bindDefault();
}

void Renderer::drawElements(RenderPrimitive primitive, uint32 indiceCount) const {
    GLenum glPrimitive = renderPrimitiveToGlEnum(primitive);
    glDrawElements(glPrimitive, indiceCount, GL_UNSIGNED_INT, 0);
}

void Renderer::drawArrays(RenderPrimitive primitive, uint32 vertexCount) const {
    GLenum glPrimitive = renderPrimitiveToGlEnum(primitive);
    glDrawArrays(glPrimitive, 0, vertexCount);
}

} // namespace R3

#endif // R3_OPENGL
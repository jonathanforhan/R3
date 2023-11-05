#if R3_OPENGL

#include "core/Renderer.hpp"
// clang-format off
#include <glad/glad.h>
#include <GLFW/glfw3.h>
// clang-format on

namespace R3 {

static auto renderPrimitiveToGlEnum(RenderPrimitive primitive) -> GLenum {
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

Renderer::Renderer() {
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
}

Renderer::~Renderer() {}

void Renderer::predraw() const {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
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
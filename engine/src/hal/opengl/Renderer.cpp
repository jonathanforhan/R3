#if R3_OPENGL

#include "core/Renderer.hpp"
// clang-format off
#include <glad/glad.h>
#include <GLFW/glfw3.h>
// clang-format on

namespace R3 {

Renderer::Renderer() {
    // glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
}

Renderer::~Renderer() {}

void Renderer::predraw() const {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Renderer::drawElements(RenderPrimitive primitive, uint32 indiceCount) const {

    GLenum glPrimitive{};
    switch (primitive) {
        case RenderPrimitive::Triangles:
            glPrimitive = GL_TRIANGLES;
            break;
        case RenderPrimitive::Lines:
            glPrimitive = GL_LINES;
            break;
        case RenderPrimitive::Points:
            glPrimitive = GL_POINTS;
            break;
    }
    glDrawElements(glPrimitive, indiceCount, GL_UNSIGNED_INT, 0);
}

} // namespace R3

#endif // R3_OPENGL
#if R3_OPENGL

#include "Renderer.hpp"
// clang-format off
#include <glad/glad.h>
#include <GLFW/glfw3.h>
// clang-format on
#include "core/Window.hpp"

namespace R3::opengl {

Renderer::Renderer(Window* window)
    : _window(window) {
    glEnable(GL_DEPTH_TEST);
}

Renderer::~Renderer() {}

void Renderer::predraw() {
    glClearColor(0.1f, 0.4f, 0.6f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Renderer::draw(RendererPrimitive primitive, uint32 n_indices) {
    uint32 uprim{};
    switch (primitive) {
        case RendererPrimitive::Triangles:
            uprim = GL_TRIANGLES;
            break;
        case RendererPrimitive::Lines:
            uprim = GL_LINES;
            break;
    }
    glDrawElements(uprim, n_indices, GL_UNSIGNED_INT, 0);
}

} // namespace R3::opengl

#endif // R3_OPENGL
#if R3_OPENGL

#include "core/Renderer.hpp"
// clang-format off
#include <glad/glad.h>
#include <GLFW/glfw3.h>
// clang-format on

#include "core/Shader.hpp"

namespace R3 {

Renderer::Renderer() {
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
}

Renderer::~Renderer() {}

void Renderer::render() {
    static const GLfloat s_bg[] = {0.2f, 0.4f, 0.3f, 1.0f};
    glClearBufferfv(GL_COLOR, 0, s_bg);
    glClear(GL_DEPTH_BUFFER_BIT);

    glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0);
}

} // namespace R3

#endif // R3_OPENGL
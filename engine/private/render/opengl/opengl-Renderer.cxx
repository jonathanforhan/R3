#if R3_OPENGL

#include "render/Renderer.hxx"
#include <glad/glad.h>

namespace R3 {

void Renderer::initialize() {}

void Renderer::deinitialize() {}

void Renderer::render() {
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
}

} // namespace R3

#endif // R3_OPENGL

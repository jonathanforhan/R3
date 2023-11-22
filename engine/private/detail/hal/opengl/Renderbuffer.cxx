#if R3_OPENGL

#include "core/Renderbuffer.hpp"
#include <glad/glad.h>
#include "api/Check.hpp"

namespace R3 {

Renderbuffer::Renderbuffer() {
    glCreateRenderbuffers(1, &m_id);
    CHECK(m_id > 0);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512);
}

} // namespace R3

#endif // R3_OPENGL

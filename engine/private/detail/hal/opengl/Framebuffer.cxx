#if R3_OPENGL

#include "core/Framebuffer.hpp"
#include <glad/glad.h>
#include "api/Check.hpp"

namespace R3 {

Framebuffer::Framebuffer() {
    glCreateFramebuffers(1, &m_id);
    CHECK(m_id > 0);
}

void Framebuffer::bind() {
    CHECK(m_id > 0);
    glBindFramebuffer(GL_FRAMEBUFFER, m_id);
}

void Framebuffer::bindDefault() {
    glBindFramebuffer(GL_FRAMEBUFFER, NULL);
}

void Framebuffer::attachRenderBuffer(const Renderbuffer& renderbuffer) {
    CHECK(m_id > 0);
    glFramebufferRenderbuffer(m_id, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, renderbuffer.bufferID());
}

} // namespace R3

#endif // R3_OPENGL
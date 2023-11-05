#if R3_OPENGL

#include "core/Texture2D.hpp"
#include <glad/glad.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include "api/Check.hpp"

namespace R3 {

Texture2D::Texture2D(std::string_view path) {
    glCreateTextures(GL_TEXTURE_2D, 1, &m_id);
    glBindTexture(GL_TEXTURE_2D, m_id);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_set_flip_vertically_on_load(true);

    int32 w, h, channels;
    uint8* data = stbi_load(path.data(), &w, &h, &channels, 0);
    CHECK(data != nullptr);

    GLint format = channels == 3 ? GL_RGB : GL_RGBA;
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(data);
}

Texture2D::~Texture2D() {
    glDeleteTextures(1, &m_id);
}

void Texture2D::bind(uint8 index) {
    glActiveTexture(GL_TEXTURE0 + index);
    glBindTexture(GL_TEXTURE_2D, m_id);
    // glBindTextureUnit(index, m_id);
}

} // namespace R3

#endif // R3_OPENGL
#if R3_OPENGL

#include "core/Texture2D.hpp"
#include <glad/glad.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include "api/Check.hpp"
#include "api/Log.hpp"

namespace R3 {

Texture2D::Texture2D(std::string_view path, TextureType type)
    : m_type(type) {
    glCreateTextures(GL_TEXTURE_2D, 1, &m_id);
    glBindTexture(GL_TEXTURE_2D, m_id);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int32 w, h, channels;
    uint8* bytes = stbi_load(path.data(), &w, &h, &channels, 0);
    CHECK(bytes != nullptr);

    GLint format = channels == 3 ? GL_RGB : GL_RGBA;
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, format, GL_UNSIGNED_BYTE, bytes);
    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(bytes);
}

Texture2D::Texture2D(uint32 width, uint32 height, const void* data, TextureType type)
    : m_type(type) {
    CHECK(data != nullptr);

    glCreateTextures(GL_TEXTURE_2D, 1, &m_id);
    glBindTexture(GL_TEXTURE_2D, m_id);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    uint32 len = height ? width * height : width;
    int32 w, h, channels;
    uint8* bytes = stbi_load_from_memory((const stbi_uc*)data, len, &w, &h, &channels, 0);
    CHECK(bytes != nullptr);

    GLint format = channels == 3 ? GL_RGB : GL_RGBA;
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, format, GL_UNSIGNED_BYTE, bytes);
    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(bytes);
}

Texture2D::~Texture2D() {
    glDeleteTextures(1, &m_id);
}

void Texture2D::bind(uint8 index) {
    glActiveTexture(GL_TEXTURE0 + index);
    glBindTexture(GL_TEXTURE_2D, m_id);
}

} // namespace R3

#endif // R3_OPENGL
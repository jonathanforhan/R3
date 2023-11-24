#if R3_OPENGL

#include "core/TextureCubeMap.hpp"
#include <glad/glad.h>
#include <stb_image.h>
#include "api/Check.hpp"

namespace R3 {

TextureCubeMap::TextureCubeMap(const TextureCubeMapCreateInfo& createInfo) {
    glCreateTextures(GL_TEXTURE_CUBE_MAP, 1, &m_id);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_id);

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    constexpr usize N_FACES = 6;
    std::string_view faces[N_FACES] = {
        createInfo.right,  //
        createInfo.left,   //
        createInfo.top,    //
        createInfo.bottom, //
        createInfo.front,  //
        createInfo.back,   //
    };

    for (uint32 i = 0; i < N_FACES; i++) {
        int32 w, h, channels;
        uint8* bytes = stbi_load(faces[i].data(), &w, &h, &channels, 0);
        CHECK(bytes != nullptr);
        GLint format = channels == 3 ? GL_RGB : GL_RGBA;
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, w, h, 0, format, GL_UNSIGNED_BYTE, bytes);
        stbi_image_free(bytes);
    }
}

TextureCubeMap::~TextureCubeMap() {
    glDeleteTextures(1, &m_id);
}

void TextureCubeMap::bind(uint8 index) {
    glActiveTexture(GL_TEXTURE0 + index);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_id);
}

} // namespace R3

#endif // R3_OPENGL
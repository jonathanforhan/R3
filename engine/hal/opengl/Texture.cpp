#include "Texture.hpp"
#include <glad/glad.h>
#include "api/Log.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include "api/Check.hpp"

namespace R3::opengl {

Texture::Texture(const char* path)
    : _id{} {
  glGenTextures(1, &_id);
  glBindTexture(GL_TEXTURE_2D, _id);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  stbi_set_flip_vertically_on_load(true);

  int32_t w, h, channels;
  unsigned char* data = stbi_load(path, &w, &h, &channels, 0);
  CHECK(data != nullptr);

  GLint format = channels == 3 ? GL_RGB : GL_RGBA;
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, format, GL_UNSIGNED_BYTE, data);
  glGenerateMipmap(GL_TEXTURE_2D);

  stbi_image_free(data);
}

void Texture::bind(uint8_t index) const {
  glActiveTexture(GL_TEXTURE0 + index);
  glBindTexture(GL_TEXTURE_2D, _id);
}

} // namespace R3::opengl

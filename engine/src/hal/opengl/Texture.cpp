#if R3_OPENGL

#include "core/Texture.hpp"
#include <glad/glad.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include "api/Check.hpp"

namespace R3 {

void Texture::assign(Texture texture) {
  if (_id == texture._id) {
    return;
  }
  destroy();
  *this = texture;
}

void Texture::destroy() {
  if (_id >= 0) {
    glDeleteTextures(1, reinterpret_cast<GLuint*>(&_id));
  }
  _id = -1;
}

void Texture::import2D(const char* path, const char *name) {
  _name = name;
  glCreateTextures(GL_TEXTURE_2D, 1, reinterpret_cast<GLuint*>(&_id));
  glBindTexture(GL_TEXTURE_2D, _id);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  stbi_set_flip_vertically_on_load(true);

  int32_t w, h, channels;
  uint8_t* data = stbi_load(path, &w, &h, &channels, 0);
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

} // namespace R3

#endif // R3_OPENGL

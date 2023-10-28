#if R3_OPENGL

#include "core/Shader.hpp"
#include <glad/glad.h>
#include <fstream>
#include <sstream>
#include <string>
#include "api/Check.hpp"
#include "api/Log.hpp"

namespace R3 {

void Shader::assign(Shader shader) {
  if (_id == shader._id) {
    return;
  }
  destroy();
  *this = shader;
}

void R3::Shader::destroy() {
  if (_id >= 0)
    glDeleteShader(_id);
  _id = -1;
}

void Shader::use() {
  CHECK(_id >= 0);
  glUseProgram(_id);
}

void Shader::import_source(const char* vert, const char* frag) {
  destroy();
  std::string vertex_string, fragment_string;
  std::ifstream vertex_file, fragment_file;

  vertex_file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
  fragment_file.exceptions(std::ifstream::failbit | std::ifstream::badbit);

  try {
    vertex_file.open(vert);
    fragment_file.open(frag);

    std::stringstream vss, fss;
    vss << vertex_file.rdbuf();
    fss << fragment_file.rdbuf();

    vertex_file.close();
    fragment_file.close();

    vertex_string = vss.str();
    fragment_string = fss.str();
  } catch (std::ifstream::failure& e) {
    LOG(Error, "shader file read failed: ", e.what());
  }

  const char* vstr = vertex_string.c_str();
  const char* fstr = fragment_string.c_str();

  int success;
  char info_log[512];

  uint32_t vertex = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertex, 1, &vstr, nullptr);
  glCompileShader(vertex);
  glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(vertex, 512, nullptr, info_log);
    LOG(Error, "vertex shader compilation failed: ", info_log);
  }

  uint32_t fragment = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragment, 1, &fstr, nullptr);
  glCompileShader(fragment);
  glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(fragment, 512, nullptr, info_log);
    LOG(Error, "fragment shader compilation failed: ", info_log);
  }

  uint32_t program = glCreateProgram();
  glAttachShader(program, vertex);
  glAttachShader(program, fragment);
  glLinkProgram(program);
  glGetProgramiv(program, GL_LINK_STATUS, &success);
  if (!success) {
    glGetProgramInfoLog(program, 512, nullptr, info_log);
    LOG(Error, "shader program linking failed: ", info_log);
  }
  glDeleteShader(vertex);
  glDeleteShader(fragment);

  this->_id = program;
}

uint32_t Shader::location(const char* uniform) const {
  return glGetUniformLocation(_id, uniform);
}

} // namespace R3

#endif // R3_OPENGL

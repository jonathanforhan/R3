#include "Shader.hpp"
#include <fstream>
#include <sstream>
#include <string>
#include "api/Log.hpp"

namespace R3::opengl {

Shader Shader::from_source(const char* vert, const char* frag) {
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
    LOG(Error, "shader file read failed: " << e.what());
  }

  const char* vstr = vertex_string.c_str();
  const char* fstr = fragment_string.c_str();

  int success;
  char info_log[512];

  unsigned vertex = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertex, 1, &vstr, nullptr);
  glCompileShader(vertex);
  glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(vertex, 512, nullptr, info_log);
    LOG(Error, "vertex shader compilation failed: " << info_log);
  }

  unsigned fragment = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragment, 1, &fstr, nullptr);
  glCompileShader(fragment);
  glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(fragment, 512, nullptr, info_log);
    LOG(Error, "fragment shader compilation failed: " << info_log);
  }

  unsigned program = glCreateProgram();
  glAttachShader(program, vertex);
  glAttachShader(program, fragment);
  glLinkProgram(program);
  glGetProgramiv(program, GL_LINK_STATUS, &success);
  if (!success) {
    glGetProgramInfoLog(program, 512, nullptr, info_log);
    LOG(Error, "shader program linking failed: " << info_log);
  }
  glDeleteShader(vertex);
  glDeleteShader(fragment);

  Shader shader;
  shader._program = program;
  return shader;
}

Shader::~Shader() {
  glDeleteShader(_program);
}

} // namespace R3::opengl

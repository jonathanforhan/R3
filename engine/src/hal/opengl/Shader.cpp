#if R3_OPENGL

#include "core/Shader.hpp"
#include <glad/glad.h>
#include <fstream>
#include <sstream>
#include <string>
#include "api/Check.hpp"
#include "api/Log.hpp"

namespace R3 {

Shader::Shader(ShaderType type, std::string_view vert, std::string_view frag) {
    switch (type) {
        case ShaderType::GLSL:
            import_glsl(vert, frag);
            break;
        case ShaderType::HLSL:
            CHECKF(type != ShaderType::HLSL, "OpenGL Renderer is incompatible with HLSL");
            break;
        case ShaderType::SPIRV:
            CHECKF(type != ShaderType::SPIRV, "OpenGL Renderer has not yet implented SPIRV");
            break;
    }
    glUseProgram(_id);
}

void Shader::destroy() {
    glDeleteShader(_id);
}

void Shader::use() const {
    glUseProgram(_id);
}

void Shader::import_glsl(std::string_view vert, std::string_view frag) {
    std::string vertex_string, fragment_string;
    std::ifstream vertex_file, fragment_file;

    vertex_file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    fragment_file.exceptions(std::ifstream::failbit | std::ifstream::badbit);

    try {
        vertex_file.open(vert.data());
        fragment_file.open(frag.data());

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

    _id = program;
}

uint32 Shader::location(std::string_view uniform) const {
    uint32_t loc = glGetUniformLocation(_id, uniform.data());
    CHECK(loc >= 0);
    return loc;
}

} // namespace R3

#endif // R3_OPENGL

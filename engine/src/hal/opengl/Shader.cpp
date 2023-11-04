#if R3_OPENGL

#include "core/Shader.hpp"
#include <fstream>
#include <sstream>
#include <glad/glad.h>
#include "api/Check.hpp"
#include "api/Log.hpp"
#include "api/Todo.hpp"

static GLuint importGLSLHelper(std::string_view shaderFile, GLenum shaderType) {
    std::string s;
    std::ifstream ifs;
    ifs.exceptions(std::ifstream::failbit | std::ifstream::badbit);

    try {
        ifs.open(shaderFile.data());
        std::stringstream ss;
        ss << ifs.rdbuf();
        ifs.close();
        s = ss.str();
    } catch (std::ifstream::failure& e) {
        LOG(Error, "shader file read failure", shaderFile.data(), e.what());
    }

    const char* pStr = s.c_str();
    int success;

    GLuint shader = glCreateShader(shaderType);
    glShaderSource(shader, 1, &pStr, nullptr);
    glCompileShader(shader);
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(shader, 512, nullptr, infoLog);
        LOG(Error, "shader compilation failure:", shaderFile.data(), infoLog);
    }

    return shader;
}

namespace R3 {

Shader::Shader(ShaderType type, std::string_view vs, std::string_view fs) {
    switch (type) {
        case ShaderType::GLSL:
            importGLSL(vs, fs);
            break;
        case ShaderType::HLSL:
            importHLSL(vs, fs);
            break;
        case ShaderType::SPIRV:
            importSPIRV(vs, fs);
            break;
    }
}

Shader::~Shader() {
    glDeleteShader(m_id);
}

void Shader::bind() {
    glUseProgram(m_id);
}

auto Shader::location(std::string_view uniform) -> uint32 const {
    uint32 location = glGetUniformLocation(m_id, uniform.data());
    CHECK(location >= 0);
    return location;
}

void Shader::importGLSL(std::string_view vs, std::string_view fs) {
    uint32 vertexShader = importGLSLHelper(vs, GL_VERTEX_SHADER);
    uint32 fragmentShader = importGLSLHelper(fs, GL_FRAGMENT_SHADER);
    int success;

    uint32 program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(program, 512, nullptr, infoLog);
        LOG(Error, "shader program linking failure", infoLog);
    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    m_id = program;
}

void Shader::importHLSL(std::string_view vs, std::string_view fs) {
    (void)vs;
    (void)fs;
    TODO("import HLSL");
}

void Shader::importSPIRV(std::string_view vs, std::string_view fs) {
    (void)vs;
    (void)fs;
    TODO("import SPIRV");
}

#if 0
template <typename T>
void Shader::writeUniform(uint32 location, T v0) {
}

template <typename T>
void Shader::writeUniform(uint32 location, T v0, T v1) {
}

template <typename T>
void Shader::writeUniform(uint32 location, T v0, T v1, T v2) {
}

template <typename T>
void Shader::writeUniform(uint32 location, T v0, T v1, T v2, T v3) {
}

template <typename T>
void Shader::writeUniform(uint32 location, const T& v0) {
}
#endif

} // namespace R3

#endif // R3_OPENGL
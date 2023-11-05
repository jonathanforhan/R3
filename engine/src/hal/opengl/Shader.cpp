#if R3_OPENGL

#include "core/Shader.hpp"
#include <glad/glad.h>
#include <fstream>
#include <sstream>
#include "api/Check.hpp"
#include "api/Log.hpp"
#include "api/Math.hpp"
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

void Shader::writeUniform(uint32 location, float v0) {
    glUniform1f(location, v0);
}

void Shader::writeUniform(uint32 location, float v0, float v1) {
    glUniform2f(location, v0, v1);
}

void Shader::writeUniform(uint32 location, float v0, float v1, float v2) {
    glUniform3f(location, v0, v1, v2);
}

void Shader::writeUniform(uint32 location, float v0, float v1, float v2, float v3) {
    glUniform4f(location, v0, v1, v2, v3);
}

void Shader::writeUniform(uint32 location, int v0) {
    glUniform1i(location, v0);
}

void Shader::writeUniform(uint32 location, int v0, int v1) {
    glUniform2i(location, v0, v1);
}

void Shader::writeUniform(uint32 location, int v0, int v1, int v2) {
    glUniform3i(location, v0, v1, v2);
}

void Shader::writeUniform(uint32 location, int v0, int v1, int v2, int v3) {
    glUniform4i(location, v0, v1, v2, v3);
}

void Shader::writeUniform(uint32 location, unsigned v0) {
    glUniform1ui(location, v0);
}

void Shader::writeUniform(uint32 location, unsigned v0, unsigned v1) {
    glUniform2ui(location, v0, v1);
}

void Shader::writeUniform(uint32 location, unsigned v0, unsigned v1, unsigned v2) {
    glUniform3ui(location, v0, v1, v2);
}

void Shader::writeUniform(uint32 location, unsigned v0, unsigned v1, unsigned v2, unsigned v3) {
    glUniform4ui(location, v0, v1, v2, v3);
}

void Shader::writeUniform(uint32 location, const glm::vec2& v0) {
    glUniform2fv(location, 1, glm::value_ptr(v0));
}

void Shader::writeUniform(uint32 location, const glm::vec3& v0) {
    glUniform3fv(location, 1, glm::value_ptr(v0));
}

void Shader::writeUniform(uint32 location, const glm::vec4& v0) {
    glUniform4fv(location, 1, glm::value_ptr(v0));
}

void Shader::writeUniform(uint32 location, const glm::ivec2& v0) {
    glUniform2iv(location, 1, glm::value_ptr(v0));
}

void Shader::writeUniform(uint32 location, const glm::ivec3& v0) {
    glUniform3iv(location, 1, glm::value_ptr(v0));
}

void Shader::writeUniform(uint32 location, const glm::ivec4& v0) {
    glUniform4iv(location, 1, glm::value_ptr(v0));
}

void Shader::writeUniform(uint32 location, const glm::uvec2& v0) {
    glUniform2uiv(location, 1, glm::value_ptr(v0));
}

void Shader::writeUniform(uint32 location, const glm::uvec3& v0) {
    glUniform3uiv(location, 1, glm::value_ptr(v0));
}

void Shader::writeUniform(uint32 location, const glm::uvec4& v0) {
    glUniform4uiv(location, 1, glm::value_ptr(v0));
}

void Shader::writeUniform(uint32 location, const glm::mat2& v0) {
    glUniformMatrix2fv(location, 1, GL_FALSE, glm::value_ptr(v0));
}

void Shader::writeUniform(uint32 location, const glm::mat3& v0) {
    glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(v0));
}

void Shader::writeUniform(uint32 location, const glm::mat4& v0) {
    glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(v0));
}

void Shader::writeUniform(uint32 location, const glm::mat2x3& v0) {
    glUniformMatrix2x3fv(location, 1, GL_FALSE, glm::value_ptr(v0));
}

void Shader::writeUniform(uint32 location, const glm::mat3x2& v0) {
    glUniformMatrix3x2fv(location, 1, GL_FALSE, glm::value_ptr(v0));
}

void Shader::writeUniform(uint32 location, const glm::mat2x4& v0) {
    glUniformMatrix2x4fv(location, 1, GL_FALSE, glm::value_ptr(v0));
}

void Shader::writeUniform(uint32 location, const glm::mat4x2& v0) {
    glUniformMatrix4x2fv(location, 1, GL_FALSE, glm::value_ptr(v0));
}

void Shader::writeUniform(uint32 location, const glm::mat3x4& v0) {
    glUniformMatrix3x4fv(location, 1, GL_FALSE, glm::value_ptr(v0));
}

void Shader::writeUniform(uint32 location, const glm::mat4x3& v0) {
    glUniformMatrix4x3fv(location, 1, GL_FALSE, glm::value_ptr(v0));
}

} // namespace R3

#endif // R3_OPENGL
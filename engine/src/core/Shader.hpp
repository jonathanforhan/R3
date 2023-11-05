#pragma once
#include <string_view>
#include "api/Types.hpp"

namespace R3 {

enum class ShaderType {
    GLSL,
    HLSL,
    SPIRV,
};

class Shader {
public:
    Shader(ShaderType type, std::string_view vs, std::string_view fs);
    Shader(const Shader&) = delete;
    Shader(Shader&& src) noexcept
        : m_id(src.m_id) {
        src.m_id = 0;
    }
    void operator=(const Shader&) = delete;
    Shader& operator=(Shader&& src) noexcept {
        m_id = src.m_id;
        src.m_id = 0;
        return *this;
    }
    ~Shader();

    void bind();
    auto location(std::string_view uniform) -> uint32 const;

    void writeUniform(uint32 location, float v0);
    void writeUniform(uint32 location, float v0, float v1);
    void writeUniform(uint32 location, float v0, float v1, float v2);
    void writeUniform(uint32 location, float v0, float v1, float v2, float v3);
    void writeUniform(uint32 location, int v0);
    void writeUniform(uint32 location, int v0, int v1);
    void writeUniform(uint32 location, int v0, int v1, int v2);
    void writeUniform(uint32 location, int v0, int v1, int v2, int v3);
    void writeUniform(uint32 location, unsigned v0);
    void writeUniform(uint32 location, unsigned v0, unsigned v1);
    void writeUniform(uint32 location, unsigned v0, unsigned v1, unsigned v2);
    void writeUniform(uint32 location, unsigned v0, unsigned v1, unsigned v2, unsigned v3);
    void writeUniform(uint32 location, const glm::vec2& v0);
    void writeUniform(uint32 location, const glm::vec3& v0);
    void writeUniform(uint32 location, const glm::vec4& v0);
    void writeUniform(uint32 location, const glm::ivec2& v0);
    void writeUniform(uint32 location, const glm::ivec3& v0);
    void writeUniform(uint32 location, const glm::ivec4& v0);
    void writeUniform(uint32 location, const glm::uvec2& v0);
    void writeUniform(uint32 location, const glm::uvec3& v0);
    void writeUniform(uint32 location, const glm::uvec4& v0);
    void writeUniform(uint32 location, const glm::mat2& v0);
    void writeUniform(uint32 location, const glm::mat3& v0);
    void writeUniform(uint32 location, const glm::mat4& v0);
    void writeUniform(uint32 location, const glm::mat2x3& v0);
    void writeUniform(uint32 location, const glm::mat3x2& v0);
    void writeUniform(uint32 location, const glm::mat2x4& v0);
    void writeUniform(uint32 location, const glm::mat4x2& v0);
    void writeUniform(uint32 location, const glm::mat3x4& v0);
    void writeUniform(uint32 location, const glm::mat4x3& v0);
    template <typename... Args>
    void writeUniform(std::string_view uniform, Args&&... args) {
        writeUniform(location(uniform), std::forward<Args>(args)...);
    }

private:
    void importGLSL(std::string_view vs, std::string_view fs);
    void importHLSL(std::string_view vs, std::string_view fs);
    void importSPIRV(std::string_view vs, std::string_view fs);

private:
    uint32 m_id{0};
};

} // namespace R3
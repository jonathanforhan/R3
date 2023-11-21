#pragma once
#include <string_view>
#include "api/Types.hpp"

namespace R3 {

enum class ShaderType {
    GLSL,
    HLSL,
    SPIRV,
};

/// @brief Shader program wrapper that can bind and unbind per draw
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

    /// @brief bind shader program
    void bind();

    /// @brief Get the location of uniform by name
    /// @param uniform uniform name
    /// @return location in bound shader
    uint32 location(std::string_view uniform) const;

    /// @brief write to uniform at given location
    /// @param location location in shader
    /// @param v0 float data
    void writeUniform(uint32 location, float v0);

    /// @brief write to uniform at given location
    /// @param location location in shader
    /// @param v0 float data
    /// @param v1 float data
    void writeUniform(uint32 location, float v0, float v1);

    /// @brief write to uniform at given location
    /// @param location location in shader
    /// @param v0 float data
    /// @param v1 float data
    /// @param v2 float data
    void writeUniform(uint32 location, float v0, float v1, float v2);

    /// @brief write to uniform at given location
    /// @param location location in shader
    /// @param v0 float data
    /// @param v1 float data
    /// @param v2 float data
    /// @param v3 float data
    void writeUniform(uint32 location, float v0, float v1, float v2, float v3);

    /// @brief write to uniform at given location
    /// @param location location in shader
    /// @param v0 int data
    void writeUniform(uint32 location, int v0);

    /// @brief write to uniform at given location
    /// @param location location in shader
    /// @param v0 int data
    /// @param v1 int data
    void writeUniform(uint32 location, int v0, int v1);

    /// @brief write to uniform at given location
    /// @param location location in shader
    /// @param v0 int data
    /// @param v1 int data
    /// @param v2 int data
    void writeUniform(uint32 location, int v0, int v1, int v2);

    /// @brief write to uniform at given location
    /// @param location location in shader
    /// @param v0 int data
    /// @param v1 int data
    /// @param v2 int data
    /// @param v3 int data
    void writeUniform(uint32 location, int v0, int v1, int v2, int v3);

    /// @brief write to uniform at given location
    /// @param location location in shader
    /// @param v0 unsigned data
    void writeUniform(uint32 location, unsigned v0);

    /// @brief write to uniform at given location
    /// @param location location in shader
    /// @param v0 unsigned data
    /// @param v1 unsigned data
    void writeUniform(uint32 location, unsigned v0, unsigned v1);

    /// @brief write to uniform at given location
    /// @param location location in shader
    /// @param v0 unsigned data
    /// @param v1 unsigned data
    /// @param v2 unsigned data
    void writeUniform(uint32 location, unsigned v0, unsigned v1, unsigned v2);

    /// @brief write to uniform at given location
    /// @param location location in shader
    /// @param v0 unsigned data
    /// @param v1 unsigned data
    /// @param v2 unsigned data
    /// @param v3 unsigned data
    void writeUniform(uint32 location, unsigned v0, unsigned v1, unsigned v2, unsigned v3);

    /// @brief write to uniform at given location
    /// @param location location in shader
    /// @param v0 vec2
    void writeUniform(uint32 location, const glm::vec2& v0);

    /// @brief write to uniform at given location
    /// @param location location in shader
    /// @param v0 vec3
    void writeUniform(uint32 location, const glm::vec3& v0);

    /// @brief write to uniform at given location
    /// @param location location in shader
    /// @param v0 vec4
    void writeUniform(uint32 location, const glm::vec4& v0);

    /// @brief write to uniform at given location
    /// @param location location in shader
    /// @param v0 ivec2
    void writeUniform(uint32 location, const glm::ivec2& v0);

    /// @brief write to uniform at given location
    /// @param location location in shader
    /// @param v0 ivec3
    void writeUniform(uint32 location, const glm::ivec3& v0);

    /// @brief write to uniform at given location
    /// @param location location in shader
    /// @param v0 ivec4
    void writeUniform(uint32 location, const glm::ivec4& v0);

    /// @brief write to uniform at given location
    /// @param location location in shader
    /// @param v0 uvec2
    void writeUniform(uint32 location, const glm::uvec2& v0);

    /// @brief write to uniform at given location
    /// @param location location in shader
    /// @param v0 uvec3
    void writeUniform(uint32 location, const glm::uvec3& v0);

    /// @brief write to uniform at given location
    /// @param location location in shader
    /// @param v0 uvec4
    void writeUniform(uint32 location, const glm::uvec4& v0);

    /// @brief write to uniform at given location
    /// @param location location in shader
    /// @param v0 mat2
    void writeUniform(uint32 location, const glm::mat2& v0);

    /// @brief write to uniform at given location
    /// @param location location in shader
    /// @param v0 mat3
    void writeUniform(uint32 location, const glm::mat3& v0);

    /// @brief write to uniform at given location
    /// @param location location in shader
    /// @param v0 mat4
    void writeUniform(uint32 location, const glm::mat4& v0);

    /// @brief write to uniform at given location
    /// @param location location in shader
    /// @param v0 mat2x3
    void writeUniform(uint32 location, const glm::mat2x3& v0);

    /// @brief write to uniform at given location
    /// @param location location in shader
    /// @param v0 mat3x2
    void writeUniform(uint32 location, const glm::mat3x2& v0);

    /// @brief write to uniform at given location
    /// @param location location in shader
    /// @param v0 mat2x4
    void writeUniform(uint32 location, const glm::mat2x4& v0);

    /// @brief write to uniform at given location
    /// @param location location in shader
    /// @param v0 mat4x2
    void writeUniform(uint32 location, const glm::mat4x2& v0);

    /// @brief write to uniform at given location
    /// @param location location in shader
    /// @param v0 mat3x4
    void writeUniform(uint32 location, const glm::mat3x4& v0);

    /// @brief write to uniform at given location
    /// @param location location in shader
    /// @param v0 mat4x3
    void writeUniform(uint32 location, const glm::mat4x3& v0);

    /// @brief Helper to allow use the pass in Shader name as a string and not have to call location explicitly
    /// @tparam ...Args type of uniform arguments
    /// @param uniform uniform name
    /// @param ...args values to pass to shader uniform
    template <typename... Args>
    void writeUniform(std::string_view uniform, Args&&... args) {
        writeUniform(location(uniform), std::forward<Args>(args)...);
    }

private:
    /// @brief helper to import GLSL shader code
    /// @param vs vertex shader
    /// @param fs fragment shader
    void importGLSL(std::string_view vs, std::string_view fs);

    /// @brief helper to import HLSL shader code
    /// @param vs vertex shader
    /// @param fs fragment shader
    void importHLSL(std::string_view vs, std::string_view fs);

    /// @brief helper to import SPIRV shader code
    /// @param vs vertex shader
    /// @param fs fragment shader
    void importSPIRV(std::string_view vs, std::string_view fs);

private:
    uint32 m_id{0};
};

} // namespace R3
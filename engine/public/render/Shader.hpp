#pragma once

#include <string_view>
#include "render/NativeRenderObject.hpp"
#include "render/RenderFwd.hpp"

namespace R3 {

class LogicalDevice;

enum class ShaderType {
    Vertex,
    Tessellation,
    Geometry,
    Fragment,
    Compute,
};

struct ShaderSpecification {
    const LogicalDevice* logicalDevice;
    std::string_view path;
};

class Shader : public NativeRenderObject {
public:
    void create(const ShaderSpecification& spec);
    void destroy();

private:
    ShaderSpecification m_spec;

#if R3_OPENGL
    uint32 location(std::string_view uniform) const;
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
#endif
};

} // namespace R3
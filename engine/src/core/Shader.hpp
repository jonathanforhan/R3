#pragma once
#include <string_view>
#include "api/Types.hpp"

namespace R3 {

enum class ShaderType { GLSL, HLSL, SPIRV };

class Shader {
public:
    Shader() = default;
    Shader(ShaderType type, std::string_view vert, std::string_view frag);
    void destroy();

    uint32 id() const { return _id; }

    uint32 location(std::string_view uniform) const;
    void write_uniform(uint32 location, float v0);
    void write_uniform(uint32 location, int v0);
    void write_uniform(uint32 location, unsigned v0);
    template <typename T>
    void write_uniform(uint32 location, T v0, T v1);
    template <typename T>
    void write_uniform(uint32 location, T v0, T v1, T v2);
    template <typename T>
    void write_uniform(uint32 location, T v0, T v1, T v2, T v3);
    template <typename T>
    void write_uniform(uint32 location, const T& v0);

private:
    void import_glsl(std::string_view vert, std::string_view frag);

private:
    uint32 _id{0};
};

} // namespace R3

#ifdef R3_OPENGL
#include "hal/opengl/Shader.ipp"
#endif

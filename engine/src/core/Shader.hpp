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

private:
    void importGLSL(std::string_view vs, std::string_view fs);
    void importHLSL(std::string_view vs, std::string_view fs);
    void importSPIRV(std::string_view vs, std::string_view fs);

    #if 0
    template <typename T>
    void writeUniform(uint32 location, T v0);
    template <typename T>
    void writeUniform(uint32 location, T v0, T v1);
    template <typename T>
    void writeUniform(uint32 location, T v0, T v1, T v2);
    template <typename T>
    void writeUniform(uint32 location, T v0, T v1, T v2, T v3);
    template <typename T>
    void writeUniform(uint32 location, const T& v0);
    #endif

private:
    uint32 m_id{0};
};

} // namespace R3
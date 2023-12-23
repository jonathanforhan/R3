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
    const LogicalDevice& logicalDevice;
    std::string_view path;
};

class Shader : public NativeRenderObject {
public:
    Shader() = default;
    Shader(const ShaderSpecification& spec);
    Shader(Shader&&) noexcept = default;
    Shader& operator=(Shader&&) noexcept = default;
    ~Shader();

private:
    Ref<const LogicalDevice> m_logicalDevice;
};

} // namespace R3
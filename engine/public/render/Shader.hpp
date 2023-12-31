#pragma once

#include <string_view>
#include "render/RenderApi.hpp"

namespace R3 {

/// @brief Shader Type
enum class R3_API ShaderType {
    Vertex,
    Tessellation,
    Geometry,
    Fragment,
    Compute,
};

/// @brief Shader Specification
struct R3_API ShaderSpecification {
    const LogicalDevice& logicalDevice; ///< LogicalDevice
    std::string_view path;              ///< Filepath to spirv shader
};

/// @brief Shader is a native shader module constructed from spirv
class R3_API Shader : public NativeRenderObject {
public:
    DEFAULT_CONSTRUCT(Shader);
    NO_COPY(Shader);
    DEFAULT_MOVE(Shader);

    /// @brief Construct Shader from spec
    /// @param spec
    Shader(const ShaderSpecification& spec);

    /// @brief Destroy Shader
    ~Shader();

private:
    Ref<const LogicalDevice> m_logicalDevice;
};

} // namespace R3
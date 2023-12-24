#pragma once

/// @file Shader.hpp

#include <string_view>
#include "render/RenderApi.hpp"

namespace R3 {

class LogicalDevice; ///< @private

/// @brief Shader Type
enum class ShaderType {
    Vertex,
    Tessellation,
    Geometry,
    Fragment,
    Compute,
};

/// @brief Shader Specification
struct ShaderSpecification {
    const LogicalDevice& logicalDevice; ///< LogicalDevice
    std::string_view path;              ///< Filepath to spirv shader
};

/// @brief Shader is a native shader module constructed from spirv
class Shader : public NativeRenderObject {
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
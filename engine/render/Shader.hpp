#pragma once

#include <filesystem>
#include <memory>
#include <span>
#include <vector>
#include "Flags.hpp"
#include "RenderHandle.hpp"
#include "engine/api/Api.hpp"
#include "engine/api/Class.hpp"
#include "engine/api/MovableHandle.hpp"
#include "engine/api/Types.hpp"

namespace R3 {

struct ShaderMetadata;

/// @brief Represents a GPU shader, providing functionality to load, manage, and access shader resources.
class R3_API Shader {
public:
    R3_CTOR_DEFAULT(Shader);
    R3_COPY_DELETE(Shader);
    R3_MOVE_DEFAULT(Shader);

    /// @brief Constructs a Shader object from a file and specifies its shader stage.
    /// @param filename The path to the shader source file.
    /// @param stage The shader stage flags indicating the type of shader (e.g., vertex, fragment).
    Shader(const std::filesystem::path& filename, ShaderStageFlags stage);

    /// @brief Destroys the Shader object and releases any associated resources.
    ~Shader() noexcept;

    /// @brief Returns the handle to the shader used for rendering.
    /// @return The handle to the shader (ShaderRenderHandle) used for rendering.
    ShaderRenderHandle shaderHandle() const noexcept { return m_shader; }

    /// @brief Returns the shader stage flags associated with the object.
    /// @return The ShaderStageFlags value representing the shader stage.
    ShaderStageFlags stage() const noexcept { return m_stage; }

    /// @brief Returns a constant reference to the shader metadata.
    /// @return A constant reference to the ShaderMetadata.
    const ShaderMetadata& metadata() const noexcept;

private:
    void createFromSource(std::span<const uint32> spirvCode);

    void populateShaderMetadata(std::span<const uint32> spirvCode);

    std::vector<uint32> readFile(const std::filesystem::path& filename) const;

private:
    MovableHandle<ShaderRenderHandle> m_shader;
    ShaderStageFlags m_stage = {};
    std::shared_ptr<ShaderMetadata> m_metadata;
};

} // namespace R3

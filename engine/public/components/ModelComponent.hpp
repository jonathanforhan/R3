#pragma once

/// @brief Provides Component for attaching mesh and material data to entity

#include <R3>
#include <variant>
#include <vector>
#include "render/model/KeyFrame.hpp"
#include "render/model/Mesh.hpp"
#include "render/model/Skin.hpp"

namespace R3 {

/// @brief ModelComponent holds Mesh data
/// A ModelComponent is constructed through the ModelLoader owned by the Renderer
class R3_API ModelComponent {
public:
    DEFAULT_CONSTRUCT(ModelComponent);
    NO_COPY(ModelComponent);
    DEFAULT_MOVE(ModelComponent);

    /// @brief Create Model by filepath
    /// @param path
    ModelComponent(const std::string& path);
    ~ModelComponent();

    /// @brief vector<Mesh> getter
    /// @return meshes
    [[nodiscard]] constexpr const std::vector<Mesh>& meshes() const { return m_meshes; }

    /// @brief vector<Mesh> getter
    /// @return meshes
    [[nodiscard]] constexpr std::vector<Mesh>& meshes() { return m_meshes; }

    /// @brief KeyFrames getter
    /// @return keyframes
    [[nodiscard]] constexpr const std::vector<KeyFrame>& keyFrames() const { return m_keyFrames; }

public:
    float currentTime = 0.0f;
    float maxTime = 0.0f;

private:
    std::vector<Mesh> m_meshes;
    std::vector<KeyFrame> m_keyFrames;

    friend class ModelLoader;
};

} // namespace R3

#pragma once

/// @brief Provides Component for attaching mesh and material data to entity

#include <R3>
#include "render/model/Animation.hpp"
#include "render/model/Mesh.hpp"
#include "render/model/Skeleton.hpp"

namespace R3 {

/// @brief ModelComponent holds Mesh data
/// A ModelComponent is constructed through the ModelLoader owned by the Renderer
struct R3_API ModelComponent {
    NO_COPY(ModelComponent);
    DEFAULT_MOVE(ModelComponent);

    /// @brief Create Model by filepath
    /// @param path
    ModelComponent(const std::string& path, const char* vertexShader = nullptr, const char* fragmentShader = nullptr);

    std::vector<Mesh> meshes;
    Skeleton skeleton;
    Animation animation;
};

} // namespace R3

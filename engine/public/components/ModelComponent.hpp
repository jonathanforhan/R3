#pragma once

/// @file ModelComponent.hpp
/// @brief Provides Component for attaching mesh and material data to entity

#include "api/Construct.hpp"
#include "api/Types.hpp"
#include "render/Mesh.hpp"

namespace R3 {

/// @brief ModelComponent holds Mesh data
/// A ModelComponent is constructed through the ModelLoader owned by the Renderer
class ModelComponent {
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
    std::vector<Mesh>& meshes() { return m_meshes; }

private:
    std::vector<Mesh> m_meshes;

    friend class ModelLoader;
};

} // namespace R3
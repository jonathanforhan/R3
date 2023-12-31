#pragma once
#pragma warning(push)
#pragma warning(disable : 4251)

/// @file ModelComponent.hpp
/// @brief Provides Component for attaching mesh and material data to entity

#include <R3>
#include <vector>
#include "render/Mesh.hpp"

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
    const std::vector<Mesh>& meshes() const { return m_meshes; }

private:
    std::vector<Mesh> m_meshes;

    friend class ModelLoader;
};

} // namespace R3

#pragma warning(pop)
#pragma once

/// @brief Provides Component for attaching mesh and material data to entity

#include <R3>
#include "render/model/KeyFrame.hpp"
#include "render/model/Mesh.hpp"
#include "render/model/ModelNode.hpp"
#include "render/model/Skeleton.hpp"
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

public:
    float currentTime = 0.0f;
    float maxTime = 0.0f;
    std::vector<ModelNode> nodes;
    std::vector<Mesh> meshes;
    std::vector<KeyFrame> keyFrames;
    Skeleton skeleton;
};

} // namespace R3

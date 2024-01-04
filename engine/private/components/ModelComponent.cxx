#include "components/ModelComponent.hpp"

#include "core/Engine.hpp"
#include "core/Scene.hpp"
#include "render/Renderer.hpp"

namespace R3 {

ModelComponent::ModelComponent(const std::string& path) {
    EngineInstance->renderer().modelLoader().load(path, *this);
}

ModelComponent::~ModelComponent() {
    for (auto& mesh : m_meshes) {
        mesh.destroy(CurrentScene);
    }
}

} // namespace R3
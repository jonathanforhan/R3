#include "components/ModelComponent.hpp"

#include "api/Log.hpp"
#include "core/Scene.hpp"
// #include "render/ModelLoader.hpp"
#include "systems/ModelSystem.hpp"

namespace R3 {

ModelComponent::ModelComponent(const std::string& path) {
    // Engine::renderer().modelLoader().load(path, *this);
    Scene::addSystem<ModelSystem>();
}

ModelComponent::~ModelComponent() {
    for (auto& mesh : m_meshes) {
        mesh.destroy();
    }
}

} // namespace R3
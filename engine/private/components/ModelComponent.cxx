#include "components/ModelComponent.hpp"

#include "api/Log.hpp"
#include "core/Scene.hpp"
#include "render/ModelLoader.hpp"
#include "systems/ModelSystem.hpp"

namespace R3 {

ModelComponent::ModelComponent(const std::string& path) {
    ModelLoader modelLoader;
    modelLoader.load(path, m_meshes, m_textures);
    Scene::addSystem<ModelSystem>();
}

} // namespace R3
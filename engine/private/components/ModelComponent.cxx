#include "components/ModelComponent.hpp"

#include "core/Engine.hpp"
#include "core/Scene.hpp"
#include "systems/AnimationSystem.hpp"

namespace R3 {

ModelComponent::ModelComponent(const std::string& path) {
    EngineInstance->renderer().modelLoader().load(path, *this);
    if (!m_keyFrames.empty()) {
        Scene::addSystem<AnimationSystem>();

        for (auto& keyFrame : m_keyFrames) {
            maxTime = std::max(keyFrame.timestamp, maxTime);
        }
    }
}

ModelComponent::~ModelComponent() {
    for (auto& mesh : m_meshes) {
        mesh.destroy(CurrentScene);
    }
}

} // namespace R3
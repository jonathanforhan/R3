#include "components/ModelComponent.hpp"

#include "core/Engine.hpp"
#include "core/Scene.hpp"
#include "systems/AnimationSystem.hpp"

namespace R3 {

ModelComponent::ModelComponent(const std::string& path) {
    EngineInstance->renderer().modelLoader().load(path, *this);
    if (!animation.keyFrames.empty()) {
        Scene::addSystem<AnimationSystem>();

        for (auto& keyFrame : animation.keyFrames) {
            animation.maxTime = std::max(keyFrame.timestamp, animation.maxTime);
        }
    }
}

} // namespace R3
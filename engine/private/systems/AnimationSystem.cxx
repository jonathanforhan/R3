#include "systems/AnimationSystem.hpp"

namespace R3 {

void AnimationSystem::tick(double dt) {
    // apply keyframe animations to all models
    auto animate = [&](ModelComponent& model) {};
    Entity::componentView<ModelComponent>().each(animate);
}

} // namespace R3
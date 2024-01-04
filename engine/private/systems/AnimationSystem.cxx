#include "systems/AnimationSystem.hpp"

namespace R3 {

void AnimationSystem::tick(double dt) {
    // apply keyframe animations to all models
    auto animate = [&](ModelComponent& model) {
        model.currentTime += dt;
        if (model.currentTime > model.maxTime) {
            model.currentTime = 0.0f;
        }

        for (usize i = 0; i < model.keyFrames().size(); i++) {
            bool greaterThanFirst = i == 0 || (model.keyFrames()[i - 1].timestamp < model.currentTime);
            bool lessThanLast =
                i == model.keyFrames().size() - 1 || (model.keyFrames()[i + 1].timestamp > model.currentTime);

            if (!(greaterThanFirst && lessThanLast)) {
                continue;
            }

            const auto& keyFrame = model.keyFrames()[i];
            auto& mesh = model.meshes()[keyFrame.node];
        }
    };
    Entity::componentView<ModelComponent>().each(animate);
}

} // namespace R3
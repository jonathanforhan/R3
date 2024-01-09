#include "systems/AnimationSystem.hpp"

#include <glm/gtx/matrix_interpolation.hpp>

namespace R3 {

void AnimationSystem::tick(double dt) {
    // apply keyframe animations to all models
    auto animate = [&](ModelComponent& model) {
        if (!model.currentAnimation) {
            model.skeleton.update(false);
            return;
        }

        CHECK(model.animations.contains(*model.currentAnimation));

        auto& animation = model.animations[*model.currentAnimation];

        animation.currentTime += float(dt);
        if (animation.currentTime >= animation.maxTime) {
            animation.currentTime = 0;
        }

        for (usize i = 0; i < animation.keyFrames.size() - 1; i++) {
            auto& keyFrame = animation.keyFrames[i];
            auto modType = keyFrame.modifierType;

            float lastTimestamp = animation.keyFrames[i].timestamp;
            float nextTimestamp = animation.keyFrames[i + 1].timestamp;

            bool gtLast = animation.currentTime >= lastTimestamp;
            bool ltNext = animation.currentTime < nextTimestamp;

            if (!(gtLast && ltNext)) {
                continue;
            }

            usize nodeIndex = animation.keyFrames[i].node;

            float interpolation = (animation.currentTime - lastTimestamp) / (nextTimestamp - lastTimestamp);

            auto& joint = model.skeleton.joints[model.skeleton.nodeToJointMap[nodeIndex]];

            if (modType == KeyFrame::Translation) {
                vec3 last = vec3(keyFrame.modifier);
                vec3 next = vec3(animation.keyFrames[i + 1].modifier);

                joint.deformedTranslation = glm::mix(last, next, interpolation);
            } else if (modType == KeyFrame::Rotation) {
                quat last = quat();
                last.x = animation.keyFrames[i].modifier.x;
                last.y = animation.keyFrames[i].modifier.y;
                last.z = animation.keyFrames[i].modifier.z;
                last.w = animation.keyFrames[i].modifier.w;

                quat next = quat();
                next.x = animation.keyFrames[i + 1].modifier.x;
                next.y = animation.keyFrames[i + 1].modifier.y;
                next.z = animation.keyFrames[i + 1].modifier.z;
                next.w = animation.keyFrames[i + 1].modifier.w;

                joint.deformedRotation = glm::normalize(glm::slerp(last, next, interpolation));
            } else if (modType == KeyFrame::Scale) {
                vec3 last = vec3(keyFrame.modifier);
                vec3 next = vec3(animation.keyFrames[i + 1].modifier);

                joint.deformedScale = glm::mix(last, next, interpolation);
            }
        }
        model.skeleton.update(true);
    };
    Entity::componentView<ModelComponent>().each(animate);
}

} // namespace R3
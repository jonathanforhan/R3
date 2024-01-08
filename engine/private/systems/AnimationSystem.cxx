#include "systems/AnimationSystem.hpp"

#include <glm/gtx/matrix_interpolation.hpp>

namespace R3 {

void AnimationSystem::tick(double dt) {
    // apply keyframe animations to all models
    auto animate = [&](ModelComponent& model) {
        if (model.animation.running) {
            model.animation.currentTime += float(dt);
            if (model.animation.currentTime >= model.animation.maxTime) {
                model.animation.currentTime = 0;
            }

            for (usize i = 0; i < model.animation.keyFrames.size() - 1; i++) {
                auto& keyFrame = model.animation.keyFrames[i];
                auto modType = keyFrame.modifierType;

                float lastTimestamp = model.animation.keyFrames[i].timestamp;
                float nextTimestamp = model.animation.keyFrames[i + 1].timestamp;

                bool gtLast = model.animation.currentTime >= lastTimestamp;
                bool ltNext = model.animation.currentTime < nextTimestamp;

                if (!(gtLast && ltNext)) {
                    continue;
                }

                usize nodeIndex = model.animation.keyFrames[i].node;

                float interpolation = (model.animation.currentTime - lastTimestamp) / (nextTimestamp - lastTimestamp);

                auto& joint = model.skeleton.joints[model.skeleton.nodeToJointMap[nodeIndex]];

                if (modType == KeyFrame::Translation) {
                    vec3 last = vec3(keyFrame.modifier);
                    vec3 next = vec3(model.animation.keyFrames[i + 1].modifier);

                    joint.deformedTranslation = glm::mix(last, next, interpolation);
                } else if (modType == KeyFrame::Rotation) {
                    quat last = quat();
                    last.x = model.animation.keyFrames[i].modifier.x;
                    last.y = model.animation.keyFrames[i].modifier.y;
                    last.z = model.animation.keyFrames[i].modifier.z;
                    last.w = model.animation.keyFrames[i].modifier.w;

                    quat next = quat();
                    next.x = model.animation.keyFrames[i + 1].modifier.x;
                    next.y = model.animation.keyFrames[i + 1].modifier.y;
                    next.z = model.animation.keyFrames[i + 1].modifier.z;
                    next.w = model.animation.keyFrames[i + 1].modifier.w;

                    joint.deformedRotation = glm::normalize(glm::slerp(last, next, interpolation));
                } else if (modType == KeyFrame::Scale) {
                    vec3 last = vec3(keyFrame.modifier);
                    vec3 next = vec3(model.animation.keyFrames[i + 1].modifier);

                    joint.deformedScale = glm::mix(last, next, interpolation);
                }
            }
        }
        model.skeleton.update(model.animation.running);
    };
    Entity::componentView<ModelComponent>().each(animate);
}

} // namespace R3
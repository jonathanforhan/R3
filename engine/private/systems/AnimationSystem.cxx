#include "systems/AnimationSystem.hpp"

#include <glm/gtx/matrix_interpolation.hpp>

namespace R3 {

void AnimationSystem::tick(double dt) {
    // apply keyframe animations to all models
    auto animate = [&](ModelComponent& model) {
        if (model.skeleton.animated) {
            model.currentTime += float(dt);
            if (model.currentTime >= model.maxTime) {
                model.currentTime = 0;
            }

            for (usize i = 0; i < model.keyFrames.size() - 1; i++) {
                auto& keyFrame = model.keyFrames[i];
                auto modType = keyFrame.modifierType;

                float lastTimestamp = model.keyFrames[i].timestamp;
                float nextTimestamp = model.keyFrames[i + 1].timestamp;

                bool gtLast = model.currentTime >= lastTimestamp;
                bool ltNext = model.currentTime < nextTimestamp;

                if (!(gtLast && ltNext)) {
                    continue;
                }

                usize nodeIndex = model.keyFrames[i].node;

                float interpolation = (model.currentTime - lastTimestamp) / (nextTimestamp - lastTimestamp);

                auto& joint = model.skeleton.joints[model.skeleton.nodeToJointMap[nodeIndex]];

                if (modType == KeyFrame::Translation) {
                    vec3 last = vec3(keyFrame.modifier);
                    vec3 next = vec3(model.keyFrames[i + 1].modifier);

                    joint.deformedTranslation = glm::mix(last, next, interpolation);
                } else if (modType == KeyFrame::Rotation) {
                    quat last = quat();
                    last.x = model.keyFrames[i].modifier.x;
                    last.y = model.keyFrames[i].modifier.y;
                    last.z = model.keyFrames[i].modifier.z;
                    last.w = model.keyFrames[i].modifier.w;

                    quat next = quat();
                    next.x = model.keyFrames[i + 1].modifier.x;
                    next.y = model.keyFrames[i + 1].modifier.y;
                    next.z = model.keyFrames[i + 1].modifier.z;
                    next.w = model.keyFrames[i + 1].modifier.w;

                    joint.deformedRotation = glm::normalize(glm::slerp(last, next, interpolation));
                } else if (modType == KeyFrame::Scale) {
                    vec3 last = vec3(keyFrame.modifier);
                    vec3 next = vec3(model.keyFrames[i + 1].modifier);

                    joint.deformedScale = glm::mix(last, next, interpolation);
                }
            }
        }
        model.skeleton.update();
    };
    Entity::componentView<ModelComponent>().each(animate);
}

} // namespace R3
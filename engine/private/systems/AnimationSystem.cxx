#include "systems/AnimationSystem.hpp"

namespace R3 {

namespace local {

void propagateNodeTranslation(ModelComponent& model, ModelNode& node, const vec3& T) {
    if (node.mesh != undefined) {
        auto& t = model.meshes[node.mesh].subTransform;
        t = glm::translate(t, T);
        for (auto child : node.children) {
            propagateNodeTranslation(model, model.nodes[child], T);
        }
    }
}

void propagateNodeRotation(ModelComponent& model, ModelNode& node, const mat4& R) {
    if (node.mesh != undefined) {
        auto& t = model.meshes[node.mesh].subTransform;
        t = R;
        for (auto child : node.children) {
            propagateNodeRotation(model, model.nodes[child], R);
        }
    }
}

void propagateNodeScale(ModelComponent& model, ModelNode& node, const vec3& S) {
    if (node.mesh != undefined) {
        auto& t = model.meshes[node.mesh].subTransform;
        t = glm::scale(t, S);
        for (auto child : node.children) {
            propagateNodeScale(model, model.nodes[child], S);
        }
    }
}

} // namespace local

void AnimationSystem::tick(double dt) {
    return;
    // apply keyframe animations to all models
    auto animate = [&](ModelComponent& model) {
        model.currentTime += float(dt);
        if (model.currentTime >= model.maxTime) {
            model.currentTime = 0;
        }

        for (usize i = 0; i < model.keyFrames.size() - 1; i++) {
            auto& keyFrame = model.keyFrames[i];

            float lastTimestamp = model.keyFrames[i].timestamp;
            float nextTimestamp = model.keyFrames[i + 1].timestamp;

            bool gtLast = model.currentTime >= lastTimestamp;
            bool ltNext = model.currentTime < nextTimestamp;

            if (gtLast && ltNext) {
                auto& node = model.nodes[model.keyFrames[i].node];

                float interpolation = (model.currentTime - lastTimestamp) / (nextTimestamp - lastTimestamp);

                if (keyFrame.modifierType == KeyFrame::Translation) {
                    auto& last = std::get<vec3>(keyFrame.modifier);
                    auto& next = std::get<vec3>(model.keyFrames[i + 1].modifier);

                    vec3 T = last + interpolation * (next - last);
                    local::propagateNodeTranslation(model, node, T);
                } else if (keyFrame.modifierType == KeyFrame::Rotation) {
                    auto& last = std::get<quat>(keyFrame.modifier);
                    auto& next = std::get<quat>(model.keyFrames[i + 1].modifier);

                    quat rot = glm::slerp(last, next, interpolation);
                    mat4 lastMat = glm::mat4_cast(last);
                    for (int j = 0; j < 4; j++) {
                        lastMat[j][j] = glm::abs(lastMat[j][j]);
                    }

                    mat4 R = lastMat * glm::mat4_cast(rot);
                    local::propagateNodeRotation(model, node, R);
                } else {
                    auto& last = std::get<vec3>(keyFrame.modifier);
                    auto& next = std::get<vec3>(model.keyFrames[i + 1].modifier);

                    vec3 S = last + interpolation * (next - last);
                    local::propagateNodeScale(model, node, S);
                }

                break;
            }
        }
    };
    Entity::componentView<ModelComponent>().each(animate);
}

} // namespace R3
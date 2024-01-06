#include "systems/AnimationSystem.hpp"

#include <glm/gtx/matrix_interpolation.hpp>

namespace R3 {

namespace local {

void propagateNodeTransform(ModelComponent& model, ModelNode& node, const mat4& t) {
    if (node.mesh != undefined) {
        model.meshes[node.mesh].subTransform = t;
        for (auto child : node.children) {
            propagateNodeTransform(model, model.nodes[child], t);
        }
    }
}

} // namespace local

void AnimationSystem::tick(double dt) {
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

                mat4& last = keyFrame.modifier;
                mat4& next = model.keyFrames[i + 1].modifier;

                mat4 t = glm::interpolate(last, next, -interpolation);
                local::propagateNodeTransform(model, node, t);
            }
        }
    };
    Entity::componentView<ModelComponent>().each(animate);
}

} // namespace R3
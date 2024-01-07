#include "render/model/Skeleton.hpp"

namespace R3 {

void Skeleton::update() {
    if (!animated) {
        for (auto& jointMatrix : finalJointsMatrices) {
            jointMatrix = mat4(1.0f);
        }
        return;
    }

    // apply animation
    for (auto i = 0; i < joints.size(); i++) {
        finalJointsMatrices[i] = joints[i].getDeformedBindMatrix();
    }

    // recursively update joint matrices
    static constexpr usize ROOT = 0;
    updateJoint(ROOT);

    // bring back to model space
    for (auto i = 0; i < joints.size(); i++) {
        finalJointsMatrices[i] = finalJointsMatrices[i] * joints[i].inverseBindMatrix;
    }
}

void Skeleton::updateJoint(usize jointIndex) {
    auto& currentJoint = joints[jointIndex];

    usize parentJoint = currentJoint.parentJoint;

    if (parentJoint != undefined) {
        finalJointsMatrices[jointIndex] = finalJointsMatrices[parentJoint] * finalJointsMatrices[jointIndex];
    }

    // recurse
    for (usize childIndex : currentJoint.children) {
        updateJoint(childIndex);
    }
}

} // namespace R3
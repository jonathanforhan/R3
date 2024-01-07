#pragma once

#include <R3>
#include <unordered_map>

namespace R3 {

/// @brief Joint belongs to Skeleton
struct Joint {
    /// @brief Skeletal Root Index
    usize rootIndex = undefined;

    /// @brief Undeform Matrix of static model
    mat4 undeformedMatrix = mat4(1.0f);

    /// @brief Inverse to go back
    mat4 inverseBindMatrix = mat4(1.0f);

    /// @brief Deformed Matrix of Animated Model
    mat4 deformedMatrix = mat4(1.0f);
    vec3 deformedTranslation = vec3(0.0f);
    quat deformedRotation = quat(1.0f, 0.0f, 0.0f, 0.0f);
    vec3 deformedScale = vec3(1.0f);

    mat4 getDeformedBindMatrix() {
        return glm::translate(glm::mat4(1.0f), deformedTranslation) * // T
               glm::mat4(deformedRotation) *                          // R
               glm::scale(glm::mat4(1.0f), deformedScale) *           // S
               undeformedMatrix;
    }

    usize parentJoint = undefined;

    /// @brief Child Joints
    std::vector<usize> children;
};

/// @brief Node Hierachy
struct Skeleton {
    void update();
    void updateJoint(usize jointIndex);

    bool animated = true;
    std::vector<Joint> joints;
    std::unordered_map<usize, usize> nodeToJointMap;
    std::vector<mat4> finalJointsMatrices; // see SSBO
};

} // namespace R3
#pragma once

#include <R3>
#include <unordered_map>

namespace R3 {

/// @brief Joint belongs to Skeleton
struct Joint {
    /// @brief Skeletal Root Index
    usize rootIndex = undefined;

    /// @brief Undeform Matrix of static model
    mat4 undeformedMatrix;

    /// @brief Inverse to go back
    mat4 inverseBindMatrix;

    /// @brief Deformed Matrix of Animated Model
    vec3 deformedTranslation;
    quat deformedRotation;
    vec3 deformedScale;

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
    void update(bool isAnimated = true);
    void updateJoint(usize jointIndex);

    std::vector<Joint> joints;
    std::unordered_map<usize, usize> nodeToJointMap;
    std::vector<mat4> finalJointsMatrices; // see SSBO
};

} // namespace R3
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

    /// @brief Deformed Matrix of Animated Model
    mat4 deformedMatrix;

    /// @brief Inverse to go back
    mat4 inverseBindMatrix;

    usize parentJoint = undefined;

    /// @brief Child Joints
    std::vector<usize> children;
};

/// @brief Node Hierachy
struct Skeleton {
    bool animated = false;
    std::vector<Joint> joints;
    std::unordered_map<usize, usize> nodeToJointMap;
    std::vector<mat4> finalJointsMatrices; // see SSBO
};

} // namespace R3
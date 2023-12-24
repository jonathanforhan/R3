#pragma once

/// @file TransformComponent.hpp
/// @brief Provides Transform data for Entities

#include "api/Construct.hpp"
#include "api/Types.hpp"

namespace R3 {

/// @brief Transform Component for all R3 and R2 transforms
/// every Entity is given a tranform at construction
struct TransformComponent : public mat4 {
    DEFAULT_CONSTRUCT(TransformComponent);

    /// @brief Construct by mat4
    /// @param m matrix4
    TransformComponent(const mat4& m)
        : mat4(m) {}

    /// @brief Assign by mat4
    /// @param m matrix4
    void operator=(const mat4& m) {
        for (auto i = 0; i < mat4::length(); i++)
            (*this)[i] = m[i];
    }
};

} // namespace R3
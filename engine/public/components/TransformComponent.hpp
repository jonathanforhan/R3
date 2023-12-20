#pragma once

#include "api/Types.hpp"

namespace R3 {

/// @brief Transform Component for all R3 and R2 transforms
/// every Entity is given a tranform at construction
struct TransformComponent : public mat4 {
    TransformComponent() = default;
    TransformComponent(const mat4& m)
        : mat4(m) {}

    void operator=(const mat4& m) {
        for (usize i = 0; i < mat4::length(); i++)
            (*this)[i] = m[i];
    }
};

} // namespace R3
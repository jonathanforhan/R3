#pragma once

#include <R3>

namespace R3 {

struct R3_API TransformComponent : public mat4 {
    DEFAULT_CONSTRUCT(TransformComponent);

    TransformComponent(const mat4& m)
        : mat4(m) {}

    void operator=(const mat4& m) {
        for (auto i = 0; i < mat4::length(); i++)
            (*this)[i] = m[i];
    }
};

} // namespace R3

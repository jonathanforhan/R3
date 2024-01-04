#pragma once

#include <R3>
#include <variant>

namespace R3 {

struct R3_API KeyFrame {
    enum ModifierType { Translation, Rotation, Scale };

    float timestamp;
    usize node;
    ModifierType modifierType;
    std::variant<vec3, quat> modifier;
};

} // namespace R3
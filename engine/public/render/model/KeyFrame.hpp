#pragma once

#include <R3>
#include <variant>

namespace R3 {

struct R3_API KeyFrame {
    enum ModifierType { Translation, Rotation, Scale };

    static ModifierType stringToModifier(std::string_view s);

    float timestamp;
    usize node = undefined;
    ModifierType modifierType;
    std::variant<vec3, quat> modifier;
};

inline KeyFrame::ModifierType KeyFrame::stringToModifier(std::string_view s) {
    if (s == "translation") {
        return KeyFrame::Translation;
    } else if (s == "rotation") {
        return KeyFrame::Rotation;
    } else if (s == "scale") {
        return KeyFrame::Scale;
    } else {
        ENSURE(false);
    }
}

} // namespace R3
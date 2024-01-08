#pragma once

#include <R3>

namespace R3 {

struct R3_API KeyFrame {
    enum ModifierType { Translation, Rotation, Scale, Weights };

    static ModifierType stringToModifier(std::string_view s);

    float timestamp = 0.0f;
    usize node = undefined;
    ModifierType modifierType;
    vec4 modifier;
};

inline KeyFrame::ModifierType KeyFrame::stringToModifier(std::string_view s) {
    if (s == "translation") {
        return KeyFrame::Translation;
    } else if (s == "rotation") {
        return KeyFrame::Rotation;
    } else if (s == "scale") {
        return KeyFrame::Scale;
    } else if (s == "weights") {
        return KeyFrame::Weights;
    }

    ENSURE(false);
}

} // namespace R3
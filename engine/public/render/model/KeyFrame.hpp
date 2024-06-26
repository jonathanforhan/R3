#pragma once

#include <R3>

namespace R3 {

struct R3_API KeyFrame {
    enum ModifierType { Translation, Rotation, Scale, Weights };

    static ModifierType stringToModifier(std::string_view s);

    float timestamp = 0.0f;
    uint32 node = undefined;
    ModifierType modifierType;
    vec4 modifier;
};

inline KeyFrame::ModifierType KeyFrame::stringToModifier(std::string_view s) {
    if (s == "translation") {
        return KeyFrame::Translation;
    }

    if (s == "rotation") {
        return KeyFrame::Rotation;
    }

    if (s == "scale") {
        return KeyFrame::Scale;
    }

    if (s == "weights") {
        return KeyFrame::Weights;
    }

    CHECK(false);
    return KeyFrame::ModifierType(int(undefined));
}

} // namespace R3

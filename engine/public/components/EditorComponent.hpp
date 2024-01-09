#pragma once

#include <R3>

namespace R3 {

struct EditorComponent {
    EditorComponent(const std::string& s)
        : name(s) {}

    std::string name;
    vec3 position = vec3(0.0f);
    vec3 rotation = vec3(0.0f);
    vec3 scale = vec3(1.0f);
};

} // namespace R3
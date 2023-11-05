#pragma once
#include "api/Types.hpp"

namespace R3 {

class CameraComponent {
public:
    CameraComponent() = default;

    void translateForward(float magnitude);
    void translateBackward(float magnitude);
    void translateRight(float magnitude);
    void translateLeft(float magnitude);
    void translateUp(float magnitude);
    void translateDown(float magnitude);
    void lookAround(float x, float y);
    void apply(mat4* view, mat4* projection, float aspectRatio) const;

    auto position() -> const vec3& { return m_position; }
    auto active() -> bool const { return m_active; }
    void setActive(bool active = true) { m_active = active; }

private:
    vec3 m_position{0.0f, 0.0f, -3.0f};
    vec3 m_front{0.0f, 0.0f, 1.0f};
    vec3 m_up{0.0f, 1.0f, 0.0f};

    float m_yaw{90};
    float m_pitch{0};
    float m_fov{45};

    bool m_active = false;
};

} // namespace R3
#pragma once

#include "Types.hpp"

namespace R3 {

enum class CameraType {
    Perspective,
    Orthographic,
};

class Camera {
public:
    explicit Camera(CameraType type = CameraType::Perspective);

    void tick(double dt);

    void translateForward(float magnitude);
    void translateBackward(float magnitude);
    void translateRight(float magnitude);
    void translateLeft(float magnitude);
    void translateUp(float magnitude);
    void translateDown(float magnitude);
    void lookAround(float x, float y);
    void lookAround(vec2 pos) { lookAround(pos.x, pos.y); }

    vec3 front() const { return m_front; }
    float fov() const { return m_fov; }
    void setFov(float fov) { m_fov = fov; }
    vec3 position() const { return m_position; }
    void setPosition(vec3 position) { m_position = position; }
    bool active() const { return m_active; }
    void setActive(bool active = true) { m_active = active; }

    void apply(float aspectRatio, ivec2 windowSize, mat4& view, mat4& projection) const;

private:
    CameraType m_cameraType;

    float m_fov{45};
    float m_yaw{90};
    float m_pitch{0};

    vec3 m_position{0.0f, 0.0f, 0.0f};
    vec3 m_front{0.0f, 0.0f, 1.0f};
    vec3 m_up{0.0f, 1.0f, 0.0f};

    bool m_active = false;

    struct ActiveKeys {
        bool w = false;
        bool a = false;
        bool s = false;
        bool d = false;
        bool e = false;
        bool q = false;
    } m_activeKeys;

    bool m_mouseDown = false;

    vec2 m_cursorPosition     = vec2(0.0f);
    vec2 m_prevCursorPosition = vec2(0.0f);
};

} // namespace R3
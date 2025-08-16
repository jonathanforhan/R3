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
    void lookAround(float dx, float dy);

    fvec3 front() const { return m_front; }
    float fov() const { return m_fov; }
    void setFov(float fov) { m_fov = fov; }
    fvec3 position() const { return m_position; }
    void setPosition(fvec3 position) { m_position = position; }
    bool active() const { return m_active; }
    void setActive(bool active = true) { m_active = active; }

    void apply(float aspectRatio, ivec2 windowSize, fmat4& view, fmat4& projection) const;

private:
    CameraType m_cameraType;

    float m_fov{45.0f};
    float m_yaw{90.0f};
    float m_pitch{0.0f};

    fvec3 m_position{0.0f, 0.0f, 0.0f};
    fvec3 m_front{0.0f, 0.0f, 1.0f};
    fvec3 m_up{0.0f, 1.0f, 0.0f};

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

    fvec2 m_cursorPosition{0.0f, 0.0f};
    fvec2 m_prevCursorPosition{0.0f, 0.0f};
};

} // namespace R3
#pragma once

#include "engine/api/Api.hpp"
#include "engine/api/Types.hpp"

namespace R3 {

enum class R3_API CameraProjectionMode {
    Perspective,
    Orthographic,
};

class R3_API Camera {
public:
    Camera();

    void update(double dt);

    void translateForward(double magnitude) noexcept;
    void translateBackward(double magnitude) noexcept;
    void translateRight(double magnitude) noexcept;
    void translateLeft(double magnitude) noexcept;
    void translateUp(double magnitude) noexcept;
    void translateDown(double magnitude) noexcept;
    void lookAround(double dx, double dy) noexcept;

    dvec3 front() const noexcept { return m_front; }
    double fov() const noexcept { return m_fov; }
    void setFov(float fov) noexcept { m_fov = fov; }
    dvec3 position() const noexcept { return m_position; }
    void setPosition(dvec3 position) noexcept { m_position = position; }
    const fmat4& projection() const noexcept { return m_projection; }
    const fmat4& view() const noexcept { return m_view; }
    void setProjectionMode(CameraProjectionMode mode) noexcept { m_projectionMode = mode; }
    bool active() const noexcept { return m_active; }
    void setActive(bool active = true) noexcept { m_active = active; }

private:
    void applyPerspective(float aspectRatio) noexcept;
    void applyOrthographic(usize2 windowSize) noexcept;

private:
    float m_fov{45.0f};
    double m_yaw{90.0};
    double m_pitch{0.0};

    dvec3 m_position{0.0, 0.0, 0.0};
    dvec3 m_front{0.0, 0.0, 1.0};
    dvec3 m_up{0.0, 1.0, 0.0};

    fmat4 m_projection{1.0f};
    fmat4 m_view{1.0f};

    CameraProjectionMode m_projectionMode{CameraProjectionMode::Perspective};

    bool m_active = false;

    // 0 = not pressed, 1 = pressed, >1 = pressed while opposite key is pressed, highest gets priority
    struct ActiveKeys {
        int w = 0;
        int a = 0;
        int s = 0;
        int d = 0;
        int e = 0;
        int q = 0;
    };
    ActiveKeys m_activeKeys;
    dvec2 m_prevCursorPosition{0.0, 0.0};
};

} // namespace R3
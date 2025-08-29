#pragma once

#include "api/Types.hpp"

namespace R3 {

enum class CameraType {
    Perspective,
    Orthographic,
};

class Camera {
public:
    explicit Camera(CameraType type = CameraType::Perspective);

    void update(double dt);

    void translateForward(float magnitude) noexcept;
    void translateBackward(float magnitude) noexcept;
    void translateRight(float magnitude) noexcept;
    void translateLeft(float magnitude) noexcept;
    void translateUp(float magnitude) noexcept;
    void translateDown(float magnitude) noexcept;
    void lookAround(float dx, float dy) noexcept;

    CameraType cameraType() const noexcept { return m_cameraType; }
    void setCameraType(CameraType type) noexcept { m_cameraType = type; }
    fvec3 front() const noexcept { return m_front; }
    float fov() const noexcept { return m_fov; }
    void setFov(float fov) noexcept { m_fov = fov; }
    fvec3 position() const noexcept { return m_position; }
    void setPosition(fvec3 position) noexcept { m_position = position; }
    bool active() const noexcept { return m_active; }
    void setActive(bool active = true) noexcept { m_active = active; }

    void apply(float aspectRatio, ivec2 windowSize, fmat4& view, fmat4& projection) const noexcept;

private:
    CameraType m_cameraType;

    float m_fov{45.0f};
    float m_yaw{90.0f};
    float m_pitch{0.0f};

    fvec3 m_position{0.0f, 0.0f, 0.0f};
    fvec3 m_front{0.0f, 0.0f, 1.0f};
    fvec3 m_up{0.0f, 1.0f, 0.0f};

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
    fvec2 m_prevCursorPosition{0.0f, 0.0f};
};

} // namespace R3
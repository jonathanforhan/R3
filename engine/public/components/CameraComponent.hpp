#pragma once

#include <R3>

namespace R3 {

struct R3_API CameraComponent {
public:
    NO_COPY(CameraComponent);

    CameraComponent();

    /// @brief Apply the camera transform to the view and projection matrices
    /// @param[out] view view matrix
    /// @param[out] projection projection matrix
    /// @param aspectRatio Window's aspect ratio
    void apply(mat4* view, mat4* projection, float aspectRatio);

    [[nodiscard]] vec3 position() const { return m_position; }
    [[nodiscard]] vec3 frontPlanar() const { return glm::normalize(vec3(m_front.x, 0, m_front.z)); }
    [[nodiscard]] vec3 backPlanar() const { return -frontPlanar(); }
    [[nodiscard]] vec3 front() const { return m_front; }
    [[nodiscard]] vec3 back() const { return -front(); }
    [[nodiscard]] vec3 right() const { return glm::cross(m_front, up); }
    [[nodiscard]] vec3 left() const { return -right(); }

public:
    float pitch = 0.0f;
    float yaw = 90.0f;
    float distance = 5.0f;
    float height = 0.1f;
    float fov = 45.0f;

    vec3 target = vec3(0.0f, 0.0f, 0.0f); // this is the target which the camera is orbiting about like a player
    vec3 up = vec3(0.0f, 1.0f, 0.0f);

    bool active = false;

private:
    vec3 m_front = vec3(0.0f, 0.0f, 1.0f);
    vec3 m_position = vec3(0.0f);
};

} // namespace R3
#pragma once
#include "api/Types.hpp"

namespace R3 {

enum class CameraType {
    Perspective,
    Orthographic,
};

/// @brief Camera component that deals with both perspective and orthographic
class CameraComponent {
public:
    /// @brief construct new camera component
    /// @param type type of camera
    CameraComponent(CameraType type = CameraType::Perspective)
        : m_cameraType(type) {}

    CameraComponent(const CameraComponent&) = delete; ///< non-copyable
    void operator=(const CameraComponent&) = delete;  ///< non-copyable

    void translateForward(float magnitude);  ///< @brief Move camera forward
    void translateBackward(float magnitude); ///< @brief Move camera backward
    void translateRight(float magnitude);    ///< @brief Move camera right
    void translateLeft(float magnitude);     ///< @brief Move camera left
    void translateUp(float magnitude);       ///< @brief Move camera up
    void translateDown(float magnitude);     ///< @brief Move camera down
    void lookAround(float x, float y);       ///< @brief Move the gimbal by x and y degrees on respective axes

    vec3 front() const { return m_front; }                     ///< @brief Get CameraComponent front vector
    float fov() const { return m_fov; }                        ///< @brief Get CameraComponent field of view
    void setFov(float fov) { m_fov = fov; }                    ///< @brief Set CameraComponent field of view
    vec3 position() const { return m_position; }               ///< @brief Get CameraComponent position
    void setPosition(vec3 position) { m_position = position; } ///< @brief Set CameraComponent position
    bool active() const { return m_active; }                   ///< @brief Get if CameraComponent is active
    void setActive(bool active = true) { m_active = active; }  ///< @brief Set CameraComponent to activek

    /// @brief Apply the camera transform to the view and projection matrices
    /// @param view view matrix
    /// @param projection projection matrix
    /// @param aspectRatio Window's aspect ratio
    void apply(mat4* view, mat4* projection, float aspectRatio) const;

private:
    CameraType m_cameraType;

    float m_fov{45};
    float m_yaw{90};
    float m_pitch{0};

    vec3 m_position{0.0f, 0.0f, 0.0f};
    vec3 m_front{0.0f, 0.0f, 1.0f};
    vec3 m_up{0.0f, 1.0f, 0.0f};

    bool m_active = false;
};

} // namespace R3
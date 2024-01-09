#pragma once

/// @file CameraComponent.hpp
/// @brief Component for viewing scenes

#include <R3>

namespace R3 {

/// @brief Types of Camera Projections
enum class R3_API EditorCameraType {
    Perspective,  ///< Perspective Projection
    Orthographic, ///< Orthographic Projection
};

/// @brief Camera Component
/// On creation if the Scene does not have a CameraSystem one will be bound to it
class R3_API EditorCameraComponent {
public:
    NO_COPY(EditorCameraComponent);

    /// @brief Create Component by type
    /// @param type
    explicit EditorCameraComponent(EditorCameraType type = EditorCameraType::Perspective);

    void translateForward(float magnitude);                 ///< Translate forward by `magnitude` amount
    void translateBackward(float magnitude);                ///< Translate backward by `magnitude` amount
    void translateRight(float magnitude);                   ///< Translate right by `magnitude` amount
    void translateLeft(float magnitude);                    ///< Translate left by `magnitude` amount
    void translateUp(float magnitude);                      ///< Translate up by `magnitude` amount
    void translateDown(float magnitude);                    ///< Translate down by `magnitude` amount
    void lookAround(float x, float y);                      ///< Move the gimbal by x and y degrees on respective axes
    void lookAround(vec2 pos) { lookAround(pos.x, pos.y); } ///< Alias

    vec3 front() const { return m_front; }                     ///< Get EditorCameraComponent front vector
    float fov() const { return m_fov; }                        ///< Get EditorCameraComponent field of view
    void setFov(float fov) { m_fov = fov; }                    ///< Set EditorCameraComponent field of view
    vec3 position() const { return m_position; }               ///< Get EditorCameraComponent position
    void setPosition(vec3 position) { m_position = position; } ///< Set EditorCameraComponent position
    bool active() const { return m_active; }                   ///< Get if EditorCameraComponent is active
    void setActive(bool active = true) { m_active = active; }  ///< Set EditorCameraComponent to activek

    /// @brief Apply the camera transform to the view and projection matrices
    /// @param[out] view view matrix
    /// @param[out] projection projection matrix
    /// @param aspectRatio Window's aspect ratio
    void apply(mat4* view, mat4* projection, float aspectRatio) const;

private:
    EditorCameraType m_cameraType;

    float m_fov{45};
    float m_yaw{90};
    float m_pitch{0};

    vec3 m_position{0.0f, 0.0f, 0.0f};
    vec3 m_front{0.0f, 0.0f, 1.0f};
    vec3 m_up{0.0f, 1.0f, 0.0f};

    bool m_active = false;
};

} // namespace R3
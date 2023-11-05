#include "CameraComponent.hpp"
#include <algorithm>
#include "api/Math.hpp"
#include "core/Engine.hpp"

namespace R3 {
void CameraComponent::translateForward(float magnitude) {
    m_position += magnitude * glm::normalize(vec3(m_front.x, 0, m_front.z));
}

void CameraComponent::translateBackward(float magnitude) {
    translateForward(-magnitude);
}

void CameraComponent::translateRight(float magnitude) {
    m_position += magnitude * glm::normalize(glm::cross(m_front, m_up));
}

void CameraComponent::translateLeft(float magnitude) {
    translateRight(-magnitude);
}

void CameraComponent::translateUp(float magnitude) {
    m_position += m_up * magnitude;
}

void CameraComponent::translateDown(float magnitude) {
    translateUp(-magnitude);
}

void CameraComponent::lookAround(float x, float y) {
    m_yaw += x;
    m_pitch += y;

    m_pitch = std::clamp(m_pitch, -89.0f, 89.0f);

    m_front.x = cos(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
    m_front.y = sin(glm::radians(m_pitch));
    m_front.z = sin(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
    m_front = glm::normalize(m_front);
}

void CameraComponent::apply(mat4* view, mat4* projection, float aspectRatio) const {
    *projection = glm::perspective(glm::radians(m_fov), aspectRatio, 0.1f, 100.0f);
    *view = glm::lookAt(m_position, m_position + m_front, m_up);
}

} // namespace R3
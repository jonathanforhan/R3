#include "components/EditorCameraComponent.hpp"

#include <algorithm>
#include "api/Math.hpp"
#include "core/Scene.hpp"
#include "systems/CameraSystem.hpp"

namespace R3 {

EditorCameraComponent::EditorCameraComponent(EditorCameraType type)
    : m_cameraType(type) {
    Scene::addSystem<CameraSystem>();
}

void EditorCameraComponent::translateForward(float magnitude) {
    m_position += magnitude * glm::normalize(vec3(m_front.x, 0, m_front.z));
}

void EditorCameraComponent::translateBackward(float magnitude) {
    translateForward(-magnitude);
}

void EditorCameraComponent::translateRight(float magnitude) {
    m_position += magnitude * glm::normalize(glm::cross(m_front, m_up));
}

void EditorCameraComponent::translateLeft(float magnitude) {
    translateRight(-magnitude);
}

void EditorCameraComponent::translateUp(float magnitude) {
    m_position += m_up * magnitude;
}

void EditorCameraComponent::translateDown(float magnitude) {
    translateUp(-magnitude);
}

void EditorCameraComponent::lookAround(float x, float y) {
    m_yaw += x;
    m_pitch += y;

    m_pitch = std::clamp(m_pitch, -89.0f, 89.0f);

    m_front.x = cos(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
    m_front.y = sin(glm::radians(m_pitch));
    m_front.z = sin(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
    m_front = glm::normalize(m_front);
}

void EditorCameraComponent::apply(mat4* view, mat4* projection, float aspectRatio) const {
    if (m_cameraType == EditorCameraType::Perspective) {
        *projection = glm::perspective(glm::radians(m_fov), aspectRatio, 0.1f, 500.0f);
    } else {
        ivec2 extent = EngineInstance->window().size();
        float denom = std::max(extent.x, extent.y) / 2.0f;
        float w = extent.x / denom;
        float h = extent.y / denom;
        *projection = glm::ortho(-w, w, -h, h, -10.0f, 500.0f);
    }
    *view = glm::lookAt(m_position, m_position + m_front, m_up);
}

} // namespace R3

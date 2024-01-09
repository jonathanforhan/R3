#include "components/CameraComponent.hpp"

#include <algorithm>
#include "api/Math.hpp"
#include "core/Scene.hpp"
#include "systems/CameraSystem.hpp"

namespace R3 {

CameraComponent::CameraComponent() {
    Scene::addSystem<CameraSystem>();
}

void CameraComponent::apply(mat4* view, mat4* projection, float aspectRatio) {
    pitch = std::clamp(pitch, -89.0f, 89.0f);

    vec3 direction = {};
    direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    direction.y = sin(glm::radians(pitch));
    direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    m_front = glm::normalize(direction);

    m_position = (target - (m_front * distance)) + vec3(0, height * 4, 0);

    *projection = glm::perspective(glm::radians(fov), aspectRatio, 0.1f, 500.0f);
    *view = glm::lookAt(m_position, m_position + m_front + vec3(0, height, 0), up);
}

} // namespace R3
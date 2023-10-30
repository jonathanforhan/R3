#include "core/Camera.hpp"
#include <algorithm>
#include "api/Math.hpp"

namespace R3 {

void Camera::translate_forward(float magnitude) {
  _position += magnitude * glm::normalize(vec3(_front.x, 0, _front.z));
}

void Camera::translate_backward(float magnitude) {
  _position -= magnitude * glm::normalize(vec3(_front.x, 0, _front.z));
}

void Camera::translate_left(float magnitude) {
  _position -= normalize(glm::cross(_front, _up)) * magnitude;
}

void Camera::translate_right(float magnitude) {
  _position += normalize(glm::cross(_front, _up)) * magnitude;
}

void Camera::translate_up(float magnitude) {
  _position += _up * magnitude;
}

void Camera::translate_down(float magnitude) {
  _position -= _up * magnitude;
}

void Camera::look_around(float x, float y) {
  _yaw += x;
  _pitch += y;

  _pitch = std::clamp(_pitch, -89.0f, 89.0f);

  _front.x = cos(glm::radians(_yaw)) * cos(glm::radians(_pitch));
  _front.y = sin(glm::radians(_pitch));
  _front.z = sin(glm::radians(_yaw)) * cos(glm::radians(_pitch));
  _front = glm::normalize(_front);
}

void Camera::apply(mat4* view, mat4* projection, float aspect_ratio) {
  *projection = glm::perspective(glm::radians(_fov), aspect_ratio, 0.1f, 100.0f);
  *view = glm::lookAt(_position, _position + _front, _up);
}

} // namespace R3

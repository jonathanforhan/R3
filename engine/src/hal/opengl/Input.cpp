#if R3_OPENGL
#include "core/Input.hpp"
#include <GLFW/glfw3.h>
#include <vector>
#include "api/Log.hpp"

namespace R3 {

static bool input_initialized = false;
static std::vector<int> active_keys;

struct mouse_state {
  bool active;
  int action;
  int mods;
};
mouse_state left_button;
mouse_state right_button;
mouse_state middle_button;

static dvec2 cursor_pos;

Input::Input(Window* window) {
  if (!input_initialized) {
    GLFWwindow* native_window = reinterpret_cast<GLFWwindow*>(window->native_ptr());

    auto key_callback = [](GLFWwindow*, int key, int scancode, int action, int mods) {
      if (action == GLFW_PRESS) {
        active_keys.push_back(key);
      } else if (action == GLFW_RELEASE) {
        active_keys.erase(std::ranges::find(active_keys, key));
      }
    };
    glfwSetKeyCallback(native_window, key_callback);

    auto mouse_callback = [](GLFWwindow*, int button, int action, int mods) {
      if (button == GLFW_MOUSE_BUTTON_LEFT) {
        left_button = {true, action, mods};
      } else if (button == GLFW_MOUSE_BUTTON_RIGHT) {
        right_button = {true, action, mods};
      } else if (button == GLFW_MOUSE_BUTTON_MIDDLE) {
        middle_button = {true, action, mods};
      }
    };
    glfwSetMouseButtonCallback(native_window, mouse_callback);

    auto cursor_callback = [](GLFWwindow*, double x, double y) { cursor_pos = {x, y}; };
    glfwSetCursorPosCallback(native_window, cursor_callback);

    input_initialized = true;
  }
  _window = window;
}

void Input::poll_keys() {
  for (int key : active_keys) {
    _key_bindings[key](static_cast<InputAction>(glfwGetKey(reinterpret_cast<GLFWwindow*>(_window->native_ptr()), key)));
  }
}

void Input::poll_mouse() {
  if (left_button.active && _mouse_bindings[GLFW_MOUSE_BUTTON_LEFT]) {
    _mouse_bindings[GLFW_MOUSE_BUTTON_LEFT](InputAction(left_button.action));
    if (left_button.action == GLFW_RELEASE) {
      left_button.active = false;
    }
  }
  if (right_button.active && _mouse_bindings[GLFW_MOUSE_BUTTON_RIGHT]) {
    _mouse_bindings[GLFW_MOUSE_BUTTON_RIGHT](InputAction(right_button.action));
    if (right_button.action == GLFW_RELEASE) {
      right_button.active = false;
    }
  }
  if (middle_button.active && _mouse_bindings[GLFW_MOUSE_BUTTON_MIDDLE]) {
    _mouse_bindings[GLFW_MOUSE_BUTTON_MIDDLE](InputAction(middle_button.action));
    if (middle_button.action == GLFW_RELEASE) {
      middle_button.active = false;
    }
  }
}

dvec2 Input::cursor_position() const {
  return cursor_pos;
}

} // namespace R3

#endif // R3_OPENGL
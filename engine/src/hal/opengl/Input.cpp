#if R3_OPENGL
#include "core/Input.hpp"
#include <GLFW/glfw3.h>
#include "api/Log.hpp"

static int s_key{}, s_scancode{}, s_action{}, s_mods{};

namespace R3 {

Input::Input(Window* window) {
  auto key_callback = [](GLFWwindow* window, int key, int scancode, int action, int mods) {
    s_key = key;
    s_scancode = scancode;
    s_action = action;
    s_mods = mods;
  };
  glfwSetKeyCallback(reinterpret_cast<GLFWwindow*>(window->native_ptr()), key_callback);
}

void Input::poll_keys() {
  if (s_key == 0) return;
  if (s_key > 0 && s_key < _key_bindings.size()) {
    if (_key_bindings[s_key]) {
      _key_bindings[s_key]((KeyAction)s_action);
    }
  } else {
    LOG(Warning, "Key:", s_key, "is not recognized by R3");
  }
  s_key = 0;
}

} // namespace R3

#endif // R3_OPENGL
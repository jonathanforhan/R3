#if R3_OPENGL
#include "core/Input.hpp"
#include <GLFW/glfw3.h>
#include <vector>
#include "api/Log.hpp"

namespace R3 {

static bool input_initialized = false;
static std::vector<int> active_keys;

struct mouse_button_state {
    bool active;
    int action;
    int mods;
};
static mouse_button_state left_button;
static mouse_button_state right_button;
static mouse_button_state middle_button;

static dvec2 cursor_pos;

Input::Input(Window* window) {
    if (!input_initialized) {
        GLFWwindow* native_window = reinterpret_cast<GLFWwindow*>(window->native_ptr());

        auto key_callback = [](GLFWwindow*, int key, int scancode, int action, int mods) {
            (void)scancode;
            (void)mods;
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

        auto cursor_callback = [](GLFWwindow* window, double x, double y) {
            int w, h;
            glfwGetFramebufferSize(window, &w, &h);
            cursor_pos = {x / w, y / w};
        };
        glfwSetCursorPosCallback(native_window, cursor_callback);

        input_initialized = true;
    }
    _window = window;
}

void Input::poll_keys() {
    for (int key : active_keys) {
        if (_key_bindings[key]) {
            _key_bindings[key](
                static_cast<InputAction>(glfwGetKey(reinterpret_cast<GLFWwindow*>(_window->native_ptr()), key)));
        }
    }
}

void Input::poll_mouse() {
    auto poll_helper = [this](mouse_button_state& button, int glfw_button) {
        if (button.active && _mouse_bindings[glfw_button]) {
            _mouse_bindings[glfw_button](InputAction(button.action));
            button.active = button.action != GLFW_RELEASE;
        }
    };
    poll_helper(left_button, GLFW_MOUSE_BUTTON_LEFT);
    poll_helper(right_button, GLFW_MOUSE_BUTTON_RIGHT);
    poll_helper(middle_button, GLFW_MOUSE_BUTTON_MIDDLE);
}

dvec2 Input::cursor_position() const {
    return cursor_pos;
}

} // namespace R3

#endif // R3_OPENGL
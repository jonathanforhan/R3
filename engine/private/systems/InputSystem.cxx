#include "systems/InputSystem.hpp"

#include <GLFW/glfw3.h>
#include <vector>
#include "api/Log.hpp"
#include "core/Engine.hpp"

static void keyCallback(GLFWwindow*, int key, int scancode, int action, int mods) {
    (void)scancode;
    (void)mods;
    if (action == GLFW_PRESS) {
        R3::Engine::pushEvent<R3::KeyPressEvent>(R3::Key(key), mods);
    } else if (action == GLFW_RELEASE) {
        R3::Engine::pushEvent<R3::KeyReleaseEvent>(R3::Key(key), mods);
    }
}

static void mouseCallback(GLFWwindow*, int button, int action, int mods) {
    switch (button) {
        case GLFW_MOUSE_BUTTON_LEFT:
            break;
        case GLFW_MOUSE_BUTTON_RIGHT:
            break;
        case GLFW_MOUSE_BUTTON_MIDDLE:
            break;
        default:
            return;
    }
}

static void cursorCallback(GLFWwindow* window, double x, double y) {
    int w, h;
    glfwGetFramebufferSize(window, &w, &h);
    // g_cursorPos = {x / w, y / w};
}

namespace R3 {

InputSystem::InputSystem() {
    GLFWwindow* window = Engine::window().handle<GLFWwindow*>();

    static bool s_initialized = false;
    if (!s_initialized) {
        glfwSetKeyCallback(window, keyCallback);
        // glfwSetMouseButtonCallback(window, mouseCallback);
        // glfwSetCursorPosCallback(window, cursorCallback);
        s_initialized = true;
    }
}

void InputSystem::tick(double) {
#if 0
    // apply key changes
    for (Key key : g_activeKeys) {
        if (!m_keyBindings[usize(key)])
            continue;
        GLFWwindow* pNativeWindow = reinterpret_cast<GLFWwindow*>(Engine::window().nativeWindow());
        int action = glfwGetKey(pNativeWindow, static_cast<int>(key));
        m_keyBindings[usize(key)](InputAction(action));
    }

    // apply mouse changes
    auto mouseHelper = [this](MouseButtonState& button, int glfwButton) {
        if (button.active && m_mouseBindings[glfwButton]) {
            m_mouseBindings[glfwButton](InputAction(button.action));
            button.active = button.action != GLFW_RELEASE;
        }
    };
    mouseHelper(g_leftButton, GLFW_MOUSE_BUTTON_LEFT);
    mouseHelper(g_rightButton, GLFW_MOUSE_BUTTON_RIGHT);
    mouseHelper(g_middleButton, GLFW_MOUSE_BUTTON_MIDDLE);
#endif 0
}

} // namespace R3

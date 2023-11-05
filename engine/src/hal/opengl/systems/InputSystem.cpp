#if R3_OPENGL

#include "systems/InputSystem.hpp"
#include <GLFW/glfw3.h>
#include <vector>
#include "api/Log.hpp"
#include "core/Engine.hpp"

struct MouseButtonState {
    bool active;
    int action;
    int mods;
};

// Global state for c callbacks
static std::vector<R3::Key> g_activeKeys;
static MouseButtonState g_leftButton;
static MouseButtonState g_rightButton;
static MouseButtonState g_middleButton;
static R3::dvec2 g_cursorPos;
static bool g_initialized = false;

static void keyCallback(GLFWwindow*, int key, int scancode, int action, int mods) {
    (void)scancode;
    (void)mods;
    if (action == GLFW_PRESS) {
        g_activeKeys.push_back(R3::Key(key));
    } else if (action == GLFW_RELEASE) {
        g_activeKeys.erase(std::ranges::find(g_activeKeys, R3::Key(key)));
    }
}

static void mouseCallback(GLFWwindow*, int button, int action, int mods) {
    switch (button) {
        case GLFW_MOUSE_BUTTON_LEFT:
            g_leftButton = {true, action, mods};
            break;
        case GLFW_MOUSE_BUTTON_RIGHT:
            g_rightButton = {true, action, mods};
            break;
        case GLFW_MOUSE_BUTTON_MIDDLE:
            g_middleButton = {true, action, mods};
            break;
        default:
            return;
    }
}

static void cursorCallback(GLFWwindow* window, double x, double y) {
    int w, h;
    glfwGetFramebufferSize(window, &w, &h);
    g_cursorPos = {x / w, y / w};
}

namespace R3 {

InputSystem::InputSystem() {
    GLFWwindow* pNativeWindow = reinterpret_cast<GLFWwindow*>(Engine::window().nativeWindow());

    static bool s_initialized = false;
    if (!s_initialized) {
        glfwSetKeyCallback(pNativeWindow, keyCallback);
        glfwSetMouseButtonCallback(pNativeWindow, mouseCallback);
        glfwSetCursorPosCallback(pNativeWindow, cursorCallback);
        s_initialized = true;
    }
}

auto InputSystem::cursorPosition() -> std::tuple<double, double> const {
    return {g_cursorPos.x, g_cursorPos.y};
}

void InputSystem::tick(double) {
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
}

} // namespace R3

#endif // R3_OPENGL
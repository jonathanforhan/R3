#include "systems/InputSystem.hpp"

#include <GLFW/glfw3.h>
#include <imgui.h>
#include "core/Scene.hpp"
#include "input/KeyboardEvent.hpp"
#include "input/MouseEvent.hpp"

namespace R3 {

namespace local {

static void keyCallback(GLFWwindow*, int key, int, int action, int mods) {
    if (ImGui::GetIO().WantCaptureKeyboard)
        return;

    switch (action) {
        case GLFW_PRESS:
            Scene::pushEvent<KeyPressEvent>(Key(key), InputModifier::Mask(mods));
            return;
        case GLFW_REPEAT:
            Scene::pushEvent<KeyRepeatEvent>(Key(key), InputModifier::Mask(mods));
            return;
        case GLFW_RELEASE:
            Scene::pushEvent<KeyReleaseEvent>(Key(key), InputModifier::Mask(mods));
            return;
    }
}

static void mouseCallback(GLFWwindow*, int button, int action, int mods) {
    if (ImGui::GetIO().WantCaptureMouse)
        return;

    switch (action) {
        case GLFW_PRESS:
            Scene::pushEvent<MouseButtonPressEvent>(MouseButton(button), InputModifier::Mask(mods));
            return;
        case GLFW_RELEASE:
            Scene::pushEvent<MouseButtonReleaseEvent>(MouseButton(button), InputModifier::Mask(mods));
            return;
    }
}

static void cursorCallback(GLFWwindow* window, double x, double y) {
    if (ImGui::GetIO().WantCaptureMouse)
        return;

    int w, h;
    glfwGetFramebufferSize(window, &w, &h);
    float posX = static_cast<float>(x) / static_cast<float>(w);
    float posY = static_cast<float>(y) / static_cast<float>(h);
    Scene::pushEvent<MouseCursorEvent>(vec2(posX, posY));
}

} // namespace local

InputSystem::InputSystem() {
    GLFWwindow* window = Engine::window().handle<GLFWwindow*>();

    static bool s_initialized = false;
    if (!s_initialized) {
        glfwSetKeyCallback(window, local::keyCallback);
        glfwSetMouseButtonCallback(window, local::mouseCallback);
        glfwSetCursorPosCallback(window, local::cursorCallback);
        s_initialized = true;
    }
}

void InputSystem::tick(double) {}

} // namespace R3

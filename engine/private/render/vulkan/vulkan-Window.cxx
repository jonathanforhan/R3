#if R3_VULKAN

#include "render/Window.hpp"

#include <imgui.h>
#include <R3>
#include <R3_input>
// clang-format off
#include <vulkan/vulkan.hpp>
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>
// clang-format on
#include "core/Scene.hpp"

namespace R3 {

Window::Window(const WindowSpecification& spec) {
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);

    const GLFWvidmode* vidmode = glfwGetVideoMode(glfwGetPrimaryMonitor());

    setHandle(glfwCreateWindow(int(vidmode->width / 1.5f), int(vidmode->height / 1.5f), spec.title.data(), NULL, NULL));
    if (!validHandle()) {
        LOG(Error, "Failed to create GLFW window");
        ENSURE(false);
    }
    glfwMaximizeWindow(handle<GLFWwindow*>());
    glfwSetWindowUserPointer(handle<GLFWwindow*>(), this);

    //--- DPI change Callback
    glfwSetWindowContentScaleCallback(handle<GLFWwindow*>(), [](GLFWwindow*, float scaleX, float scaleY) {
        Scene::pushEvent<WindowContentScaleChangeEvent>(scaleX, scaleY);
    });

    //--- Error Handling Callback
    glfwSetErrorCallback([](int errorCode, const char* errorMessage) { LOG(Error, "code", errorCode, errorMessage); });

    //--- Resize Callback
    glfwSetFramebufferSizeCallback(handle<GLFWwindow*>(), [](GLFWwindow* window, int width, int height) {
        if (Scene::topEvent() == HASH32("on-window-resize")) {
            Scene::popEvent();
        }
        Scene::pushEvent<WindowResizeEvent>(width, height);
        auto* _this = reinterpret_cast<decltype(this)>(glfwGetWindowUserPointer(window));
        _this->setShouldResize(true);
    });

    //--- Keyboard Input Callback
    glfwSetKeyCallback(handle<GLFWwindow*>(), [](GLFWwindow*, int key, int, int action, int mods) {
        if (ImGui::GetIO().WantCaptureKeyboard && action != GLFW_RELEASE)
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
    });

    //--- MouseButton Callback
    glfwSetMouseButtonCallback(handle<GLFWwindow*>(), [](GLFWwindow*, int button, int action, int mods) {
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
    });

    //--- Cursor Input Callback
    glfwSetCursorPosCallback(handle<GLFWwindow*>(), [](GLFWwindow* window, double x, double y) {
        if (ImGui::GetIO().WantCaptureMouse)
            return;

        int w, h;
        glfwGetFramebufferSize(window, &w, &h);
        float posX = static_cast<float>(x) / static_cast<float>(w);
        float posY = static_cast<float>(y) / static_cast<float>(h);
        Scene::pushEvent<MouseCursorEvent>(vec2(posX, posY));
        Scene::setCursorPosition(vec2(x, y));
    });

    //--- Window Close Callback
    glfwSetWindowCloseCallback(handle<GLFWwindow*>(), [](GLFWwindow*) { Scene::pushEvent<WindowCloseEvent>(); });
}

Window::~Window() {
    glfwDestroyWindow(handle<GLFWwindow*>());
    glfwTerminate();
}

void Window::show() {
    glfwShowWindow(handle<GLFWwindow*>());
}

void Window::hide() {
    glfwHideWindow(handle<GLFWwindow*>());
}

bool Window::isVisible() const {
    return glfwGetWindowAttrib(handle<GLFWwindow*>(), GLFW_VISIBLE);
}

void Window::resize(ivec2 extent) {
    glfwSetWindowSize(handle<GLFWwindow*>(), extent.x, extent.y);
}

void Window::resize(int32 width, int32 height) {
    glfwSetWindowSize(handle<GLFWwindow*>(), width, height);
}

ivec2 Window::size() const {
    int32 w, h;
    glfwGetFramebufferSize(handle<GLFWwindow*>(), &w, &h);
    return ivec2(w, h);
}

void Window::size(int32& width, int32& height) const {
    glfwGetFramebufferSize(handle<GLFWwindow*>(), &width, &height);
}

ivec2 Window::position() const {
    int x, y;
    glfwGetWindowPos(handle<GLFWwindow*>(), &x, &y);
    return ivec2(x, y);
}

void Window::position(int32& x, int32& y) const {
    glfwGetWindowPos(handle<GLFWwindow*>(), &x, &y);
}

void Window::setPosition(ivec2 position) {
    glfwSetWindowPos(handle<GLFWwindow*>(), position.x, position.y);
}

void Window::setPosition(int32 x, int32 y) {
    glfwSetWindowPos(handle<GLFWwindow*>(), x, y);
}

float Window::aspectRatio() const {
    ivec2 extent = size();
    return static_cast<float>(extent.x) / static_cast<float>(extent.y);
}

vec2 Window::contentScale() const {
    float x, y;
    glfwGetWindowContentScale(handle<GLFWwindow*>(), &x, &y);
    return vec2(x, y);
}

bool Window::shouldClose() const {
    return glfwWindowShouldClose(handle<GLFWwindow*>());
}

void Window::update() {
    glfwPollEvents();
}

void* Window::nativeId() const {
#ifdef WIN32
    return (void*)glfwGetWin32Window(handle<GLFWwindow*>());
#else
    return (void*)glfwGetX11Window(handle<GLFWwindow*>());
#endif // WIN32
}

void Window::kill() {
    glfwSetWindowShouldClose(handle<GLFWwindow*>(), GLFW_TRUE);
}

} // namespace R3

#endif // R3_VULKAN

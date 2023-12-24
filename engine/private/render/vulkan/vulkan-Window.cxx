#if R3_VULKAN

#include "render/Window.hpp"

// clang-format off
#include <vulkan/vulkan.hpp>
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>
// clang-format on
#include "api/Ensure.hpp"
#include "api/Hash.hpp"
#include "api/Log.hpp"
#include "api/Version.hpp"
#include "core/Scene.hpp"
#include "input/WindowEvent.hpp"

namespace R3 {

Window::Window(const WindowSpecification& spec) {
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);

    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* vidmode = glfwGetVideoMode(monitor);

    setHandle(glfwCreateWindow(vidmode->width / 2, vidmode->height / 2, spec.title.data(), nullptr, nullptr));
    if (handle() == nullptr) {
        LOG(Error, "Failed to create GLFW window");
        ENSURE(false);
    }
    glfwMakeContextCurrent(handle<GLFWwindow*>());
    glfwMaximizeWindow(handle<GLFWwindow*>());
    glfwSetWindowUserPointer(handle<GLFWwindow*>(), this);

    glfwSetFramebufferSizeCallback(handle<GLFWwindow*>(), [](GLFWwindow* window, int width, int height) {
        if (Scene::topEvent() == HASH32("on-window-resize"))
            Scene::popEvent();
        Scene::pushEvent<WindowResizeEvent>(width, height);

        auto* _this = reinterpret_cast<decltype(this)>(glfwGetWindowUserPointer(window));
        _this->m_shouldResize = true;
    });

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

void Window::resize(int32 width, int32 height) {
    glfwSetWindowSize(handle<GLFWwindow*>(), width, height);
}

std::tuple<int32, int32> Window::size() const {
    int32 w, h;
    glfwGetFramebufferSize(handle<GLFWwindow*>(), &w, &h);
    return {w, h};
}

void Window::size(int32& width, int32& height) const {
    glfwGetFramebufferSize(handle<GLFWwindow*>(), &width, &height);
}

float Window::aspectRatio() const {
    auto [width, height] = size();
    return static_cast<float>(width) / static_cast<float>(height);
}

bool Window::shouldClose() const {
    return glfwWindowShouldClose(handle<GLFWwindow*>());
}

void Window::update() {
    glfwPollEvents();
}

void* Window::nativeId() const {
#ifdef WIN32
    return glfwGetWin32Window(handle<GLFWwindow*>());
#else
    return glfwGetWinX11Window(handle<GLFWwindow*>());
#endif // WIN32
}

void Window::kill() {
    glfwSetWindowShouldClose(handle<GLFWwindow*>(), GLFW_TRUE);
}

} // namespace R3

#endif // R3_VULKAN

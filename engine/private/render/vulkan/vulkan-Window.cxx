#if R3_VULKAN

#include "render/Window.hpp"
// clang-format off
#include <vulkan/vulkan.hpp>
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>
// clang-format on
#include <vector>
#include "api/Ensure.hpp"
#include "api/Log.hpp"
#include "api/Version.hpp"

namespace R3 {

Window::Window(const WindowSpecification& spec)
    : m_spec(spec) {
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_SAMPLES, 4);

    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* vidmode = glfwGetVideoMode(monitor);

    glfwWindowHint(GLFW_RED_BITS, vidmode->redBits);
    glfwWindowHint(GLFW_GREEN_BITS, vidmode->greenBits);
    glfwWindowHint(GLFW_BLUE_BITS, vidmode->blueBits);
    glfwWindowHint(GLFW_REFRESH_RATE, vidmode->refreshRate);
    glfwWindowHint(GLFW_AUTO_ICONIFY, GLFW_TRUE);
    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);

    const int32 w = static_cast<int>(vidmode->width * 0.75);
    const int32 h = static_cast<int>(vidmode->height * 0.75);

    setHandle(glfwCreateWindow(w, h, m_spec.title.data(), nullptr, nullptr));
    if (handle() == nullptr) {
        LOG(Error, "Failed to create GLFW window");
        ENSURE(false);
    }
    glfwMakeContextCurrent(handle<GLFWwindow*>());
    glfwSetWindowPos(handle<GLFWwindow*>(), vidmode->width / 2 - w / 2, vidmode->height / 2 - h / 2);
    glfwSwapInterval(GLFW_TRUE);
    glfwSetWindowUserPointer(handle<GLFWwindow*>(), this);

    glfwSetFramebufferSizeCallback(handle<GLFWwindow*>(), [](GLFWwindow* window, int, int) {
        Window* self = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
        self->setShouldResize(true);
    });
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

bool Window::shouldResize() const {
    return m_shouldResize;
}

void Window::setShouldResize(bool b) {
    m_shouldResize = b;
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

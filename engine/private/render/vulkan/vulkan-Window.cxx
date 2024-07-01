#if R3_VULKAN

#include "render/Window.hpp"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <api/Log.hpp>
#include <api/Types.hpp>
#include <cassert>
#include <cstdlib>

#if WIN32
#define GLFW_EXPOSE_NATIVE_WIN32 1
#else
#define GLFW_EXPOSE_NATIVE_X11 1
#endif
#include <GLFW/glfw3native.h>

namespace R3 {

Window& Window::instance() {
    static Window s_window;
    return s_window;
}

void Window::initialize() {
    assert(!m_window); // only initialize once

    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    // glfwWindowHint(GLFW_SCALE_TO_MONITOR, GLFW_TRUE);
    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);

    const GLFWvidmode* vidmode = glfwGetVideoMode(glfwGetPrimaryMonitor());

    const int width   = static_cast<int>(vidmode->width / 1.5f);
    const int height  = static_cast<int>(vidmode->height / 1.5f);
    const char* title = "R3";

    if (!(m_window = glfwCreateWindow(width, height, title, nullptr, nullptr))) {
        R3_LOG(Error, "window creation failure");
        std::exit(EXIT_FAILURE);
    }
    glfwMakeContextCurrent(m_window);
    glfwSetWindowUserPointer(m_window, this);

    auto resizeCallback = [](GLFWwindow* window, int, int) {
        auto* self = reinterpret_cast<decltype(this)>(glfwGetWindowUserPointer(window));
        self->setShouldResize();
    };
    glfwSetFramebufferSizeCallback(m_window, resizeCallback);

    auto errorCallback = [](int code, const char* msg) { R3_LOG(Error, "glfw error code: {}, {}", code, msg); };
    glfwSetErrorCallback(errorCallback);
}

void Window::deinitialize() {
    glfwDestroyWindow(m_window);
    glfwTerminate();
}

void Window::show() {
    glfwShowWindow(m_window);
}

void Window::hide() {
    glfwHideWindow(m_window);
}

ivec2 Window::size() const {
    int32 w, h;
    glfwGetWindowSize(m_window, &w, &h);
    return ivec2(w, h);
}

void Window::size(int32* width, int32* height) const {
    glfwGetWindowSize(m_window, width, height);
}

int32 Window::width() const {
    int32 width;
    size(&width, nullptr);
    return width;
}

int32 Window::height() const {
    int32 height;
    size(nullptr, &height);
    return height;
}

void Window::resize(ivec2 extent) {
    glfwSetWindowSize(m_window, extent.x, extent.y);
}

void Window::resize(int32 width, int32 height) {
    glfwSetWindowSize(m_window, width, height);
}

ivec2 Window::position() const {
    int x, y;
    glfwGetWindowPos(m_window, &x, &y);
    return ivec2(x, y);
}

void Window::position(int32* x, int32* y) const {
    glfwGetWindowPos(m_window, x, y);
}

void Window::setPosition(ivec2 position) {
    glfwSetWindowPos(m_window, position.x, position.y);
}

void Window::setPosition(int32 x, int32 y) {
    glfwSetWindowPos(m_window, x, y);
}

float Window::aspectRatio() const {
    vec2 extent(static_cast<vec2>(size()));
    return extent.x / extent.y;
}

fvec2 Window::contentScale() const {
    float x, y;
    glfwGetWindowContentScale(m_window, &x, &y);
    return fvec2(x, y);
}

bool Window::isMinimized() const {
    int32 width, height;
    size(&width, &height);
    return !(width || height);
}

bool Window::shouldClose() const {
    return glfwWindowShouldClose(m_window);
}

void Window::update() {
    glfwPollEvents();
}

void* Window::native() {
#if WIN32
    return glfwGetWin32Window(m_window);
#else
    return glfwGetX11Window(m_window);
#endif
}

void Window::kill() {
    glfwSetWindowShouldClose(m_window, GLFW_TRUE);
}

} // namespace R3

#endif // R3_VULKAN
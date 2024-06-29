#if R3_OPENGL

#include "render/Window.hpp"

#include <api/Assert.hpp>
#include <api/Log.hpp>
#include <api/Types.hpp>
#include <api/Version.hpp>
#include <cstdlib>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#if WIN32
#define GLFW_EXPOSE_NATIVE_WIN32 1
#else
#define GLFW_EXPOSE_NATIVE_X11 1
#endif
#include <GLFW/glfw3native.h>

namespace R3 {

Window::Window()
    : m_window(nullptr) {}

Window& Window::instance() {
    static Window s_window;
    return s_window;
}

void Window::initialize() {
    ASSERT(!m_window); // only initialize once

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, OPENGL_VERSION_MAJOR);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, OPENGL_VERSION_MINOR);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SCALE_TO_MONITOR, GLFW_TRUE);
    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
#endif

    const GLFWvidmode* vidmode = glfwGetVideoMode(glfwGetPrimaryMonitor());

    const int width   = static_cast<int>(vidmode->width / 1.5f);
    const int height  = static_cast<int>(vidmode->height / 1.5f);
    const char* title = "R3";

    // creation
    if (!(m_window = glfwCreateWindow(width, height, title, nullptr, nullptr))) {
        LOG(Error, "window creation failure");
        exit(-1);
    }
    glfwMakeContextCurrent(m_window);

    // resize
    auto resizeCallback = [](GLFWwindow*, int width, int height) { glViewport(0, 0, width, height); };
    glfwSetFramebufferSizeCallback(m_window, resizeCallback);

    // error handling
    auto errorCallback = [](int code, const char* msg) { R3_LOG(Error, "glfw error code: {}, {}", code, msg); };
    glfwSetErrorCallback(errorCallback);

    // opengl
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        R3_LOG(Error, "glad initialization failure");
        exit(-1);
    }
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

ivec2 Window::size() {
    int32 w, h;
    glfwGetWindowSize(m_window, &w, &h);
    return ivec2(w, h);
}

void Window::size(int32& width, int32& height) {
    glfwGetWindowSize(m_window, &width, &height);
}

void Window::resize(ivec2 extent) {
    glfwSetWindowSize(m_window, extent.x, extent.y);
}

void Window::resize(int32 width, int32 height) {
    glfwSetWindowSize(m_window, width, height);
}

ivec2 Window::position() {
    int x, y;
    glfwGetWindowPos(m_window, &x, &y);
    return ivec2(x, y);
}

void Window::position(int32& x, int32& y) {
    glfwGetWindowPos(m_window, &x, &y);
}

void Window::setPosition(ivec2 position) {
    glfwSetWindowPos(m_window, position.x, position.y);
}

void Window::setPosition(int32 x, int32 y) {
    glfwSetWindowPos(m_window, x, y);
}

float Window::aspectRatio() {
    vec2 extent(static_cast<vec2>(size()));
    return extent.x / extent.y;
}

fvec2 Window::contentScale() {
    float x, y;
    glfwGetWindowContentScale(m_window, &x, &y);
    return fvec2(x, y);
}

bool Window::shouldClose() {
    return glfwWindowShouldClose(m_window);
}

void Window::update() {
    glfwSwapBuffers(m_window);
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

#endif // R3_OPENGL

#if R3_OPENGL

#include "core/Window.hpp"
// clang-format off
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>
// clang-format on
#include "api/Ensure.hpp"
#include "api/Log.hpp"
#include "api/Version.hpp"

#define GLWIN(_Win) (reinterpret_cast<GLFWwindow*>(_Win))

namespace R3 {

Window::Window(std::string_view title) {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, OPENGL_VERSION_MAJOR);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, OPENGL_VERSION_MINOR);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* vidmode = glfwGetVideoMode(monitor);

    glfwWindowHint(GLFW_RED_BITS, vidmode->redBits);
    glfwWindowHint(GLFW_GREEN_BITS, vidmode->greenBits);
    glfwWindowHint(GLFW_BLUE_BITS, vidmode->blueBits);
    glfwWindowHint(GLFW_REFRESH_RATE, vidmode->refreshRate);
    glfwWindowHint(GLFW_AUTO_ICONIFY, GLFW_TRUE);
    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);

    int32 w = static_cast<int>(vidmode->width * 0.75);
    int32 h = static_cast<int>(vidmode->height * 0.75);

    m_nativeWindow = glfwCreateWindow(w, h, title.data(), nullptr, nullptr);
    if (!m_nativeWindow) {
        LOG(Error, "Failed to create GLFW window");
        ENSURE(false);
    }
    glfwSetWindowPos(GLWIN(m_nativeWindow), vidmode->width / 2 - w / 2, vidmode->height / 2 - h / 2);
    glfwMakeContextCurrent(GLWIN(m_nativeWindow));

    auto resizeCallback = [](GLFWwindow*, int width, int height) { glViewport(0, 0, width, height); };
    glfwSetFramebufferSizeCallback(GLWIN(m_nativeWindow), resizeCallback);

    glfwSwapInterval(GL_FALSE);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        LOG(Error, "Failed to initialize GLAD");
        ENSURE(false);
    }
}

Window::~Window() {
    glfwDestroyWindow(GLWIN(m_nativeWindow));
    glfwTerminate();
}

void Window::show() {
    glfwShowWindow(GLWIN(m_nativeWindow));
}

void Window::hide() {
    glfwHideWindow(GLWIN(m_nativeWindow));
}

auto Window::isVisible() const -> bool const {
    return glfwGetWindowAttrib(GLWIN(m_nativeWindow), GLFW_VISIBLE);
}

void Window::resize(int32 width, int32 height) {
    glfwSetWindowSize(GLWIN(m_nativeWindow), width, height);
}

auto Window::size() const -> std::tuple<int32, int32> const {
    int32 w, h;
    glfwGetFramebufferSize(GLWIN(m_nativeWindow), &w, &h);
    return {w, h};
}

void Window::size(int32& width, int32& height) const {
    glfwGetFramebufferSize(GLWIN(m_nativeWindow), &width, &height);
}

auto Window::aspectRatio() const -> float const {
    auto [width, height] = size();
    return static_cast<float>(width) / static_cast<float>(height);
}

auto Window::shouldClose() const -> bool const {
    return glfwWindowShouldClose(GLWIN(m_nativeWindow));
}

void Window::update() {
    glfwSwapBuffers(GLWIN(m_nativeWindow));
    glfwPollEvents();
}

auto Window::nativeId() const -> void* const {
#ifdef WIN32
    return glfwGetWin32Window(GLWIN(m_nativeWindow));
#else
    return glfwGetWinX11Window(GLWIN(m_nativeWindow));
#endif // WIN32
}

auto Window::nativeWindow() const -> void* const {
    return m_nativeWindow;
}

void Window::kill() {
    glfwSetWindowShouldClose(GLWIN(m_nativeWindow), GLFW_TRUE);
}

} // namespace R3

#endif // R3_OPENGL
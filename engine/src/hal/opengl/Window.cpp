#if R3_OPENGL

#include "core/Window.hpp"
// clang-format off
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>
// clang-format on
#include "api/Log.hpp"

#define GLWIN(_Win) (reinterpret_cast<GLFWwindow*>(_Win))

namespace R3 {

Window::Window(std::string_view title) {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
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

    int w = static_cast<int>(vidmode->width * 0.75);
    int h = static_cast<int>(vidmode->height * 0.75);

    m_pNativeWindow = glfwCreateWindow(w, h, title.data(), nullptr, nullptr);
    if (!m_pNativeWindow) {
        LOG(Error, "Failed to create GLFW window");
        glfwTerminate();
    }
    glfwSetWindowPos(GLWIN(m_pNativeWindow), vidmode->width / 2 - w / 2, vidmode->height / 2 - h / 2);
    glfwMakeContextCurrent(GLWIN(m_pNativeWindow));

    auto resizeCallback = [](GLFWwindow*, int width, int height) { glViewport(0, 0, width, height); };
    glfwSetFramebufferSizeCallback(GLWIN(m_pNativeWindow), resizeCallback);

#if not R3_BUILD_DIST
    glfwSwapInterval(GL_FALSE);
#else
    glfwSwapInterval(GL_TRUE);
#endif

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        LOG(Error, "Failed to initialize GLAD");
    }
}

Window::~Window() {
    glfwDestroyWindow(GLWIN(m_pNativeWindow));
    glfwTerminate();
}

void Window::show() {
    glfwShowWindow(GLWIN(m_pNativeWindow));
}

void Window::hide() {
    glfwHideWindow(GLWIN(m_pNativeWindow));
}

auto Window::isVisible() -> bool const {
    return glfwGetWindowAttrib(GLWIN(m_pNativeWindow), GLFW_VISIBLE);
}

void Window::resize(int32 width, int32 height) {
    glfwSetWindowSize(GLWIN(m_pNativeWindow), width, height);
}

auto Window::size() -> std::tuple<int32, int32> const {
    int32 w, h;
    glfwGetFramebufferSize(GLWIN(m_pNativeWindow), &w, &h);
    return {w, h};
}

void Window::size(int32& width, int32& height) const {
    glfwGetFramebufferSize(GLWIN(m_pNativeWindow), &width, &height);
}

auto Window::aspectRatio() -> float const {
    auto [width, height] = size();
    return static_cast<float>(width) / static_cast<float>(height);
}

auto Window::shouldClose() -> bool const {
    return glfwWindowShouldClose(GLWIN(m_pNativeWindow));
}

void Window::update() {
    glfwSwapBuffers(GLWIN(m_pNativeWindow));
    glfwPollEvents();
}

auto Window::nativeId() -> void* const {
#ifdef WIN32
    return glfwGetWin32Window(GLWIN(m_pNativeWindow));
#else
    return glfwGetWinX11Window(GLWIN(m_pNativeWindow));
#endif // WIN32
}

auto Window::nativeWindow() -> void* const {
    return m_pNativeWindow;
}

void Window::kill() {
    glfwSetWindowShouldClose(GLWIN(m_pNativeWindow), GLFW_TRUE);
}

} // namespace R3

#endif // R3_OPENGL
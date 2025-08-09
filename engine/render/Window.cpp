#include "render/Window.hpp"

#if WIN32
#define GLFW_EXPOSE_NATIVE_WIN32 1
#else
#define GLFW_EXPOSE_NATIVE_X11 1
#endif
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

#include <string_view>
#include "Exception.hpp"
#include "Log.hpp"
#include "Types.hpp"

namespace R3 {

Window::Window() {
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    GLFWmonitor* primary = glfwGetPrimaryMonitor();

    float content_scale_x, content_scale_y;
    glfwGetMonitorContentScale(primary, &content_scale_x, &content_scale_y);

    static constexpr float scale = 1.5f;

    const GLFWvidmode* vidmode = glfwGetVideoMode(primary);
    const int width            = static_cast<int>(vidmode->width / (content_scale_x * scale));
    const int height           = static_cast<int>(vidmode->height / (content_scale_y * scale));
    static const char* title   = "R3";

    if (!(m_window = glfwCreateWindow(width, height, title, nullptr, nullptr))) {
        glfwTerminate();
        throw Exception("GLFW window creation failure");
    }
    glfwMakeContextCurrent(m_window);
    glfwSwapInterval(1);
    glfwSetWindowUserPointer(m_window, this);

    auto resize_callback = [](GLFWwindow* window, int width, int height) {
        (void)width; // TODO
        (void)height;
        auto* _this = reinterpret_cast<decltype(this)>(glfwGetWindowUserPointer(window));
        _this->setShouldResize(true);
    };
    glfwSetFramebufferSizeCallback(m_window, resize_callback);

    auto error_callback = [](int code, const char* msg) { LOG_ERROR("glfw error code: {}, {}", code, msg); };
    glfwSetErrorCallback(error_callback);
}

Window::~Window() {
    if (m_window) {
        glfwDestroyWindow(m_window);
        glfwTerminate();
        m_window = nullptr;
    }
}

void Window::setTitle(std::string_view title) {
    glfwSetWindowTitle(m_window, title.data());
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

int32 Window::width() const {
    int32 width;
    glfwGetWindowSize(m_window, &width, nullptr);
    return width;
}

int32 Window::height() const {
    int32 height;
    glfwGetWindowSize(m_window, nullptr, &height);
    return height;
}

void Window::setSize(ivec2 extent) {
    glfwSetWindowSize(m_window, extent.x, extent.y);
}

void Window::setSize(int32 width, int32 height) {
    glfwSetWindowSize(m_window, width, height);
}

ivec2 Window::position() const {
    int x, y;
    glfwGetWindowPos(m_window, &x, &y);
    return ivec2(x, y);
}

void Window::setPosition(ivec2 position) {
    glfwSetWindowPos(m_window, position.x, position.y);
}

void Window::setPosition(int32 x, int32 y) {
    glfwSetWindowPos(m_window, x, y);
}

float Window::aspectRatio() const {
    vec2 extent{static_cast<vec2>(size())};
    return extent.x / extent.y;
}

fvec2 Window::contentScale() const {
    float x, y;
    glfwGetWindowContentScale(m_window, &x, &y);
    return fvec2(x, y);
}

bool Window::isMinimized() const {
    int32 width, height;
    glfwGetWindowSize(m_window, &width, &height);
    return !(width && height);
}

bool Window::shouldClose() const {
    return glfwWindowShouldClose(m_window);
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

void* Window::native() {
#if WIN32
    return glfwGetWin32Window(m_window);
#else
    return glfwGetX11Window(m_window);
#endif
}

GLFWwindow* Window::glfw() {
    return m_window;
}

const GLFWwindow* Window::glfw() const {
    return m_window;
}

void Window::kill() {
    glfwSetWindowShouldClose(m_window, GLFW_TRUE);
}

} // namespace R3
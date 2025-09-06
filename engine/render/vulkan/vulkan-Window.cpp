#include "render/Window.hpp"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "api/Exception.hpp"

namespace R3 {

Window::Window() {
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
    glfwWindowHint(GLFW_REFRESH_RATE, GLFW_DONT_CARE);
    // glfwWindowHint(GLFW_SCALE_TO_MONITOR, GLFW_TRUE);

    GLFWmonitor* primary = glfwGetPrimaryMonitor();

    int monitorX, monitorY;
    glfwGetMonitorPos(primary, &monitorX, &monitorY);

    float contentScaleX, contentScaleY;
    glfwGetMonitorContentScale(primary, &contentScaleX, &contentScaleY);

    const GLFWvidmode* vidmode = glfwGetVideoMode(primary);
    const int width            = static_cast<int>(vidmode->width / contentScaleX);
    const int height           = static_cast<int>(vidmode->height / contentScaleY);
    static const char* title   = "R3";

    if (!(m_window = glfwCreateWindow(width, height, title, nullptr, nullptr))) {
        glfwTerminate();
        throw Exception("GLFW window creation failure");
    }

    glfwMakeContextCurrent(m_window);
    glfwSwapInterval(1);
    glfwSetWindowUserPointer(m_window, this);
    glfwSetInputMode(m_window, GLFW_STICKY_KEYS, GLFW_TRUE);

    // center glfw window on create
    const int centerX = monitorX + (vidmode->width - width) / 2;
    const int centerY = monitorY + (vidmode->height - height) / 2;
    glfwSetWindowPos(m_window, centerX, centerY);
}

Window::~Window() noexcept {
    if (m_window) {
        glfwDestroyWindow(m_window);
        glfwTerminate();
        m_window = nullptr;
    }
}

} // namespace R3

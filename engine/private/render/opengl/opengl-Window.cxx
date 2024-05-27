#if R3_OPENGL

#include "render/Window.hpp"

#include "api/Log.hpp"
#include "api/Version.hpp"
#include <cstdlib>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace R3 {

Window* window = nullptr; // must be initialized by Window::initialize

Window::Window()
    : m_window(nullptr) {}

Window::~Window() {
    glfwDestroyWindow(window->m_window);
    glfwTerminate();
}

void Window::initialize() {
    static Window s_window;
    window = &s_window;

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, OPENGL_VERSION_MAJOR);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, OPENGL_VERSION_MINOR);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SCALE_TO_MONITOR, GLFW_TRUE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
#endif

    const GLFWvidmode* vidmode = glfwGetVideoMode(glfwGetPrimaryMonitor());

    const int width   = static_cast<int>(vidmode->width / 1.5f);
    const int height  = static_cast<int>(vidmode->height / 1.5f);
    const char* title = "R3";

    // creation
    if (!(window->m_window = glfwCreateWindow(width, height, title, nullptr, nullptr))) {
        LOG(Error, "window creation failure");
        exit(-1);
    }
    glfwMakeContextCurrent(window->m_window);

    // resize
    auto resizeCallback = [](GLFWwindow*, int width, int height) { glViewport(0, 0, width, height); };
    glfwSetFramebufferSizeCallback(window->m_window, resizeCallback);

    // error handling
    auto errorCallback = [](int code, const char* msg) { LOG(Error, "{} {}", code, msg); };
    glfwSetErrorCallback(errorCallback);

    // opengl
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        LOG(Error, "glad initialization failure");
        exit(-1);
    }

    // render loop
    while (!glfwWindowShouldClose(window->m_window)) {
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        glfwSwapBuffers(window->m_window);
        glfwPollEvents();
    }
}

} // namespace R3

#endif // R3_OPENGL

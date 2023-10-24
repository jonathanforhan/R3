#include "Window.hpp"

namespace R3::opengl {

Window::Window(int32_t width, int32_t height, const char* title)
    : IWindow(width, height, title) {
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

  _window = glfwCreateWindow(width, height, title, nullptr, nullptr);
  if (!_window) {
    fputs("Failed to create GLFW window", stderr);
    glfwTerminate();
  }

  glfwMakeContextCurrent(_window);

  auto resize_callback = [](GLFWwindow*, int width, int height) {
    glViewport(0, 0, width, height);
  };
  glfwSetFramebufferSizeCallback(_window, resize_callback);

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    fputs("Failed to initialize GLAD", stderr);
  }
}

Window::~Window() {
  glfwDestroyWindow(_window);
  glfwTerminate();
}

void Window::set_visible(bool b) {
  b ? glfwShowWindow(_window) : glfwHideWindow(_window);
}

bool Window::is_visible() const {
  return true;
}

void Window::resize(int32_t width, int32_t height) {
  (void)width;
  (void)height;
  return;
}

bool Window::should_close() const {
  return glfwWindowShouldClose(_window);
}

void Window::update() {
  glfwSwapBuffers(_window);
}

} // namespace R3::opengl

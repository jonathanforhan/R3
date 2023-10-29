#if R3_OPENGL

#include "Window.hpp"
// clang-format off
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>
// clang-format on
#include "api/Log.hpp"

namespace R3::opengl {

Window::Window(const char* title) {
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

  _window = glfwCreateWindow(w, h, title, nullptr, nullptr);
  if (!_window) {
    LOG(Error, "Failed to create GLFW window");
    glfwTerminate();
  }
  glfwSetWindowPos(_window, vidmode->width / 2 - w / 2, vidmode->height / 2 - h / 2);
  glfwMakeContextCurrent(_window);

  auto resize_callback = [](GLFWwindow*, int width, int height) { glViewport(0, 0, width, height); };
  glfwSetFramebufferSizeCallback(_window, resize_callback);

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    LOG(Error, "Failed to initialize GLAD");
  }
}

Window::~Window() {
  glfwDestroyWindow(_window);
  glfwTerminate();
}

void Window::show() {
  glfwShowWindow(_window);
}

void Window::hide() {
  glfwHideWindow(_window);
}

bool Window::is_visible() const {
  return glfwGetWindowAttrib(_window, GLFW_VISIBLE);
}

void Window::resize(int32_t width, int32_t height) {
  glfwSetWindowSize(_window, width, height);
}

float Window::aspect_ratio() const {
  int width, height;
  glfwGetWindowSize(_window, &width, &height);
  return static_cast<float>(width) / static_cast<float>(height);
}

bool Window::should_close() const {
  return glfwWindowShouldClose(_window);
}

void Window::update() {
  glfwSwapBuffers(_window);
  glfwPollEvents();
}

void* Window::native_id() const {
#ifdef WIN32
  return glfwGetWin32Window(_window);
#else
  return glfwGetWinX11Window(_window);
#endif // WIN32
}

void* Window::native_ptr() const {
  return _window;
}

void Window::kill() {
  glfwSetWindowShouldClose(_window, true);
}

} // namespace R3::opengl

#endif // R3_OPENGL

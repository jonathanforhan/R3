#pragma once
// clang-format off
#include <glad/glad.h>
#include <GLFW/glfw3.h>
// clang-format on
#include <cstdint>
#include <iostream>
#include "../IWindow.hpp"

namespace R3::opengl {

class Window : IWindow {
public:
  Window(int32_t width, int32_t height, const char* title);
  ~Window();

  void set_visible(bool b) override;
  bool is_visible() const override;
  void resize(int32_t width, int32_t height) override;
  bool should_close() const override;
  void update() override;

private:
  GLFWwindow *_window;
};

} // namespace R3::opengl


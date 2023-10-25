#pragma once
#include "../IWindow.hpp"

struct GLFWwindow;

namespace R3::opengl {

class Window : IWindow {
public:
  Window(int32_t width, int32_t height, const char* title);
  ~Window();

  void show() override;
  void hide() override;
  bool is_visible() const override;
  void resize(int32_t width, int32_t height) override;
  bool should_close() const override;
  void update() override;

private:
  GLFWwindow *_window;
};

} // namespace R3::opengl


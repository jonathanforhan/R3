#pragma once
#include "hal/IWindow.hpp"

struct GLFWwindow;

namespace R3::opengl {

class Window : IWindow {
public:
  explicit Window(const char* title);
  ~Window();

  void show() override;
  void hide() override;
  bool is_visible() const override;
  void resize(int32_t width, int32_t height) override;
  float aspect_ratio() const override;
  bool should_close() const override;
  void update() override;

private:
  GLFWwindow* _window;
};

} // namespace R3::opengl

#pragma once
#if R3_OPENGL

#include "hal/IWindow.hpp"

struct GLFWwindow;

namespace R3 {
class Engine;
}

namespace R3::opengl {

class Window : protected IWindow {
private:
  explicit Window(const char* title);
  Window(const Window&) = delete;
  void operator=(const Window&) = delete;
  friend class R3::Engine;

public:
  ~Window();

  void show() override;
  void hide() override;
  bool is_visible() const override;
  void resize(int32_t width, int32_t height) override;
  float aspect_ratio() const override;
  bool should_close() const override;
  void update() override;
  void* native_id() const override;
  void* native_ptr() const override;
  void kill() override;

private:
  GLFWwindow* _window;
};

} // namespace R3::opengl

#endif // R3_OPENGL

#pragma once
#include <cstdint>

namespace R3 {

class IWindow {
protected:
  virtual void show() = 0;
  virtual void hide() = 0;
  virtual bool is_visible() const = 0;
  virtual void resize(int32_t width, int32_t height) = 0;
  virtual float aspect_ratio() const = 0;
  virtual bool should_close() const = 0;
  virtual void update() {}
};

} // namespace R3

#pragma once
#include <cstdint>

namespace R3 {

class IWindow {
protected:
  IWindow(int32_t width, int32_t height, const char* title)
      : _width(width),
        _height(height),
        _title(title) {}

  virtual void set_visible(bool b) = 0;
  virtual bool is_visible() const = 0;
  virtual void resize(int32_t width, int32_t height) = 0;
  virtual bool should_close() const = 0;
  virtual void update() {};

  int32_t width() const { return _width; }
  int32_t height() const { return _width; }
  const char* title() const { return _title; }

protected:
  int32_t _width;
  int32_t _height;
  const char* _title;
};

} // namespace R3

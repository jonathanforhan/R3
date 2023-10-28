#pragma once
#include <cstdint>

namespace R3 {

class Texture {
public:
  Texture() = default;
  Texture(const char* path, const char* name) { import2D(path, name); }

  void assign(Texture texture);
  void destroy();
  void import2D(const char* path, const char* name);
  void bind(uint8_t index) const;
  bool valid() const { return _id >= 0; }

  const char* name() const { return _name; }
  void rename(const char* name) { _name = name; }

private:
  int64_t _id;
  const char* _name;
};

} // namespace R3

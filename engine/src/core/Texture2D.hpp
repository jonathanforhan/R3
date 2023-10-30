#pragma once
#include "api/Types.hpp"

namespace R3 {

class Texture2D {
public:
  Texture2D() = default;
  Texture2D(const std::string_view path, std::string_view name);
  void destroy();

  uint32 id() const { return _id; }
  std::string_view name() const { return _name; }
  void rename(std::string_view name) { _name = name; }

private:
  uint32 _id{0};
  std::string _name;
};

} // namespace R3

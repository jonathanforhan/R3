#include "Actor.hpp"
#include "core/Engine.hpp"

namespace R3 {

R3::Actor::Actor()
    : model(mat4(1.0f)),
      _shader(),
      _texture(),
      _mesh() {}

Actor::~Actor() {
  _shader.destroy();
  _texture.destroy();
  _mesh.destroy();
}

} // namespace R3

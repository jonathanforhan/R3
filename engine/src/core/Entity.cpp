#include "Entity.hpp"
#include "core/Engine.hpp"

namespace R3 {

Entity::Entity()
    : model(mat4(1.0f)),
      _shader(),
      _texture(),
      _mesh() {}

Entity::~Entity() {
  _shader.destroy();
  _texture.destroy();
  _mesh.destroy();
}

} // namespace R3

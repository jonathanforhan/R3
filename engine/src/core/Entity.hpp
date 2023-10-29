#pragma once
#include "api/Math.hpp"
#include "core/Mesh.hpp"
#include "core/Shader.hpp"
#include "core/Texture.hpp"

namespace R3 {

class Entity {
public:
  Entity();
  virtual ~Entity();

  void set_mesh(Mesh mesh) { _mesh.assign(mesh); }
  void set_shader(Shader shader) { _shader.assign(shader); }
  void set_texture(Texture texture) { _texture.assign(texture); }

  virtual void tick(double delta_time) { (void)delta_time; };

private:
  template <typename T>
  void write_to_shader(const char* name, T data) {
    _shader.write_uniform(_shader.location(name), data);
  }
  template <typename T>
  void write_to_shader(uint32_t location, T data) {
    _shader.write_uniform(location, data);
  }

public:
  mat4 model;

public:
  Mesh _mesh;
  Shader _shader;
  Texture _texture;

  friend class Engine;
};

} // namespace R3
#pragma once
#include "api/Types.hpp"
#include <vector>

namespace R3 {

class Component;

class Entity {
public:
  Entity()
      : transform(mat4(1.0f)) {}
  virtual ~Entity() {}
  virtual void tick(double){};

  const std::vector<Component*>& components() { return _components; }
  void add_component(Component* component);
  void remove_component(Component* component);

public:
  mat4 transform;

private:
  std::vector<Component*> _components;
};

} // namespace R3
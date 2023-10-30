#pragma once 

namespace R3 {

class Entity;

class Component {
public:
  Component() = default;
  virtual ~Component() {}

  virtual void initialize() {}
  virtual void deinitialize() {}
  virtual void tick(double) {}

  Entity* parent{nullptr};
};

} // namespace R3
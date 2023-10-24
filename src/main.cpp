#include "Engine.hpp"

int main(void) {
  R3::Engine& engine = R3::Engine::instance();
  return engine.run();
}

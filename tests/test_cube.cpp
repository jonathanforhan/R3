#include "Engine.hpp"
#include "Log.hpp"

int main(void) {
  LOG(Info, "Running Cube Test...");
  R3::Engine& engine = R3::Engine::instance();
  return engine.run();
}

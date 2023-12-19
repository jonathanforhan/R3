#include <R3>
#include "input/KeyboardEvent.hpp"
#include "input/WindowEvent.hpp"
#include "systems/InputSystem.hpp"

using namespace R3;

void runScene() {
    Engine::renderer()._Model = ModelComponent("assets/DamagedHelmet/glTF-Binary/DamagedHelmet.glb");

    Engine::bindEventListener([](WindowResizeEvent& e) { printf("%ux%u\n", e.payload.width, e.payload.height); });
    Engine::bindEventListener([](KeyPressEvent& e) { printf("press: %c\n", e.payload.key); });
    Engine::bindEventListener([](KeyRepeatEvent& e) { printf("repeat: %c\n", e.payload.key); });
    Engine::bindEventListener([](KeyReleaseEvent& e) { printf("release: %c\n", e.payload.key); });

    InputSystem in;

    Engine::loop();
}

int main() {
    runScene();
    return 0;
}
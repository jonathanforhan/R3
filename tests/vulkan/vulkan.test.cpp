#include <R3>
#include <format>
#include "input/Event.hpp"
#include "input/KeyboardEvent.hpp"
#include "systems/InputSystem.hpp"

using namespace R3;

void runScene() {
    Engine::renderer()._Model = ModelComponent("assets/DamagedHelmet/glTF-Binary/DamagedHelmet.glb");

    //--- The code required to setup those callbacks... SO EASY!!!
    Engine::bindEventListener([](WindowResizeEvent& event) -> void {
        std::cout << std::format("{}x{}", event.payload.width, event.payload.height);
    });
    Engine::bindEventListener([](KeyPressEvent& event) { std::printf("%c", event.payload.key); });
    Engine::bindEventListener([](KeyReleaseEvent& event) { std::printf("%c", (int)event.payload.key + ('a' - 'A')); });
    //---

    InputSystem in;

    Engine::loop();
}

int main() {
    runScene();
    return 0;
}
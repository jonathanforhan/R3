#include <R3>
#include <format>
#include "event/Event.hpp"

using namespace R3;

void runScene() {
    Engine::renderer()._Model = ModelComponent("assets/DamagedHelmet/glTF-Binary/DamagedHelmet.glb");
    Engine::bindEventListener([](Event<HASH32("on-resize"), WindowResizePayload>& event) {
        std::printf("%s\n", std::format("{}x{}", event.payload.width, event.payload.height).c_str());
    });
    Engine::loop();
}

int main() {
    runScene();
    return 0;
}
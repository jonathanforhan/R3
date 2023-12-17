#include <R3>
#include "event/Event.hpp"

using namespace R3;

void runScene() {
    Engine::renderer()._Model = ModelComponent("assets/DamagedHelmet/glTF-Binary/DamagedHelmet.glb");
    Engine::loop();
}

int main(int argc, char** argv) {
    static constexpr uuid _A = "Hello World"_HASH;
    static constexpr uuid _B = HASH("Hello World");
    static_assert(_A == _B);

    runScene();
    return 0;
}
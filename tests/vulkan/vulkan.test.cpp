#include "core/Engine.hpp"
#include "api/Log.hpp"

using namespace R3;

void runScene() {
    Engine::window().show();

    while (!Engine::window().shouldClose()) {
        Engine::window().update();
    }
}

int main() {
    try {
        runScene();
    } catch (std::exception& e) {
        LOG(Error, e.what());
    }
    return 0;
}
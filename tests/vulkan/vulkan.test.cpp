#include <R3>
#include "render/Vertex.hpp"
#include "core/BasicGeometry.hpp"
#include "api/Log.hpp"

using namespace R3;

void runScene() {
    Engine::loop();
}

int main() {
    try {
        runScene();
    } catch (std::exception& e) {
        LOG(Error, e.what());
    }
    return 0;
}
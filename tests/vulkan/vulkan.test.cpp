#include <R3>
#include "render/Vertex.hpp"
#include "core/BasicGeometry.hpp"
#include "api/Log.hpp"

using namespace R3;

void runScene() {
    Engine::renderer()._Model = ModelComponent("assets/DamagedHelmet/glTF-Binary/DamagedHelmet.glb");
    Engine::loop();
}

int main() {
        runScene();

    return 0;
}
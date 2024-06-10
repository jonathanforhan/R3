#include "core/Engine.hpp"

#include "render/Renderer.hxx"
#include "render/Window.hpp"

namespace R3 {

Engine& Engine::instance() {
    static Engine s_engine;
    return s_engine;
}

void Engine::initialize() {
    Window::instance().initialize();
}

void Engine::deinitialize() {
    Window::instance().deinitialize();
}

int Engine::loop() {
    auto& window = Window::instance();
    Renderer renderer(window);

    window.show();

    while (!window.shouldClose()) {
        renderer.render();
        window.update();
    }

    return 0;
}

} // namespace R3
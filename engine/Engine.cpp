#include "Engine.hpp"

#include <chrono>
#include "EventHandler.hpp"
#include "Types.hpp"
#include "render/Renderer.hpp"
#include "render/Window.hpp"

#include "Log.hpp"
#include "input/InputEvents.hpp"

namespace R3 {

Engine& Engine::instance() noexcept {
    static Engine instance;
    return instance;
}

double Engine::deltaTime() {
    using namespace std::chrono;

    static auto s_prev = system_clock::now();

    const auto curr = system_clock::now();
    const double dt = duration<double>(curr - s_prev).count();

    s_prev = curr;
    return dt;
}

int Engine::run() {
    Window window;
    Renderer renderer(window);

    window.show();

    while (!window.shouldClose()) {
        const double dt = deltaTime();

        window.update();

        EventHandler::instance().dispatchEvents();

        renderer.render(dt);
    }

    return 0;
}

} // namespace R3

#include "core/Engine.hpp"

#include "render/Renderer.hxx"
#include "render/Window.hpp"
#include <api/Log.hpp>
#include <api/Result.hpp>
#include <expected>
#include <type_traits>

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

Result<void> Engine::loop() {
    auto& window = Window::instance();

    auto renderer_ = Renderer::create(window);
    if (!renderer_) {
        return std::unexpected(renderer_.error());
    }
    Renderer renderer = std::move(*renderer_);

    window.show();

    while (!window.shouldClose()) {
        renderer.render();
        window.update();
    }

    return Result<void>();
}

} // namespace R3
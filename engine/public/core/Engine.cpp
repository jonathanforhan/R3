#include "core/Engine.hpp"

#include <chrono>
#include "core/Scene.hpp"
#include "render/Renderer.hxx"
#include "render/ResourceManager.hxx"
#include "render/Window.hxx"

namespace R3 {

Engine* EngineInstance = nullptr;
Scene* CurrentScene = nullptr;

#if not R3_DLL_IMPL
Engine::Engine() {
    try {
        auto* window = new Window({"R3"});
        auto* renderer = new Renderer({*window});
        m_windowView = WindowView(window);
        m_renderView = RenderView(renderer);
    } catch (std::exception const& e) {
        LOG(Error, e.what());
    }
}
#endif

void Engine::loop() {
    auto& window = *reinterpret_cast<Window*>(m_windowView.handle());
    auto& renderer = *reinterpret_cast<Renderer*>(m_renderView.handle());

    window.show();

    while (!window.shouldClose() || !Scene::isEventQueueEmpty()) {
        double dt = deltaTime();

        Scene::dispatchEvents();
        Scene::runSystems(dt);

        renderer.setView(CurrentScene->view());
        renderer.setProjection(CurrentScene->projection());

        renderer.render();

        window.update();
    }

    renderer.waitIdle();

    CurrentScene->clearRegistry();
}

double Engine::deltaTime() {
    using namespace std::chrono;

    static auto s_prev = system_clock::now();
    auto now = system_clock::now();
    double dt = duration<double>(now - s_prev).count();
    s_prev = now;
    return dt;
}

} // namespace R3

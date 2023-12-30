#include "core/Engine.hpp"

#include <chrono>
#include <filesystem>
#include "core/Scene.hpp"
#include "input/WindowEvent.hpp"
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

EngineStatusCode Engine::loop(const char* dlName) {
    auto& window = *reinterpret_cast<Window*>(m_windowView.handle());
    auto& renderer = *reinterpret_cast<Renderer*>(m_renderView.handle());

    window.show();

    EngineStatusCode code = EngineStatusCode::Success;

#if not R3_BUILD_DIST
    int frameCounter = 0;
#endif

    while ((!window.shouldClose() && code == EngineStatusCode::Success) || !Scene::isEventQueueEmpty()) {
        double dt = deltaTime();

        Scene::dispatchEvents();
        Scene::runSystems(dt);

        renderer.setView(CurrentScene->view());
        renderer.setProjection(CurrentScene->projection());

        renderer.render();

        window.update();

#if not R3_BUILD_DIST
        frameCounter++;

        // poll for new dl every 60 frames
        if (frameCounter == 60) {
            if (std::filesystem::exists(dlName)) {
                LOG(Info, "new client dl detected");
                code = EngineStatusCode::DlOutOfData;
            }

            frameCounter = 0;
        }
#endif
    }

    renderer.waitIdle();

    CurrentScene->clearRegistry();

    return code;
}

void Engine::resetRenderer() {
    auto* renderer = reinterpret_cast<Renderer*>(m_renderView.handle());
    delete renderer;
    m_renderView = RenderView(new Renderer({*reinterpret_cast<Window*>(m_windowView.handle())}));
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

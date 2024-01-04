#include "core/Engine.hpp"

#include <chrono>
#include <filesystem>
#include <thread>
#include "core/Scene.hpp"
#include "render/Renderer.hpp"
#include "render/ResourceManager.hxx"
#include "render/Window.hpp"

namespace R3 {

Engine* EngineInstance = nullptr;
Scene* CurrentScene = nullptr;

#if not R3_DLL_IMPL
Engine::Engine()
    : m_window({"R3"}),
      m_renderer({m_window}) {}
#endif

EngineStatusCode Engine::loop(const char* dlName) {
    using namespace std::chrono;
    using namespace std::chrono_literals;

    EngineStatusCode code = EngineStatusCode::Success;

#if not R3_BUILD_DIST
    int frameCounter = 0;
#endif

    m_window.show();

    m_renderer.preLoop();

    while ((!m_window.shouldClose() && code == EngineStatusCode::Success) || !Scene::isEventQueueEmpty()) {
        double dt = deltaTime();

        Scene::dispatchEvents();
        Scene::runSystems(dt);

        m_renderer.setView(CurrentScene->view());
        m_renderer.setProjection(CurrentScene->projection());
        m_renderer.setCursorPosition(CurrentScene->cursorPosition());

        m_renderer.renderEditorInterface(dt);
        m_renderer.render();

        m_window.update();

#if not R3_BUILD_DIST
        frameCounter = (frameCounter + 1) % 60; // poll for new dl every 60 frames
        if (frameCounter == 0 && std::filesystem::exists(dlName)) {
            code = EngineStatusCode::DlOutOfData;
        }
#endif
        // std::this_thread::sleep_for(microseconds(int64(8'333 - (dt * 1'000))));
    }

    m_renderer.waitIdle();

    return code;
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

#include "core/Engine.hpp"

#include <chrono>
#include "api/Memory.hxx"
#include "input/WindowEvent.hpp"

namespace R3 {

Engine::Engine()
    : m_window({.title = "R3"}),
      m_renderer({.window = m_window}),
      m_activeScene(nullptr) {
    m_eventArena.reserve(KILOBYTE * 5);
    bindEventListenerHelper([this](WindowResizeEvent&) { m_renderer.resize(); });
}

Scene& Engine::addScene(const std::string& name, bool setActive) {
    auto& engine = Engine::inst();
    engine.m_scenes.try_emplace(name);
    if (setActive) {
        setActiveScene(name);
    }
    return engine.m_scenes.at(name);
}

void Engine::removeScene(const std::string& name) {
    auto& engine = Engine::inst();
    auto it = engine.m_scenes.find(name);
    if (it != engine.m_scenes.end()) {
        engine.m_scenes.erase(it);
    }
}

Scene& Engine::activeScene() {
    return *Engine::inst().m_activeScene;
}

Scene& Engine::getScene(const std::string& name) {
    return Engine::inst().m_scenes.at(name);
}

bool Engine::isActiveScene(const std::string& name) {
    auto& engine = Engine::inst();
    return &engine.m_scenes.at(name) == engine.m_activeScene;
}

void Engine::setActiveScene(const std::string& name) {
    auto& engine = Engine::inst();
    engine.m_activeScene = &engine.m_scenes.at(name);
}

void Engine::loop() {
    auto& engine = Engine::inst();
    engine.m_window.show();
    while (!engine.m_window.shouldClose()) {
        engine.dispatchEvents();
        CHECK(engine.m_eventArena.size() == 0);
        // engine.m_activeScene->runSystems(engine.deltaTime());
        engine.m_renderer.render(engine.deltaTime());
        engine.m_window.update();
    }
    engine.m_renderer.waitIdle();
}

Engine& Engine::inst() {
    static Engine s_instance;
    return s_instance;
}

double Engine::deltaTime() const {
    using namespace std::chrono;

    static auto s_prev = system_clock::now();
    auto now = system_clock::now();
    double dt = duration<double>(now - s_prev).count();
    s_prev = now;
    return dt;
}

void Engine::dispatchEvents() {
    while (!m_eventQueue.empty()) {
        void* const p = m_eventQueue.front().first;
        const uuid32 id = *(uuid32*)p;

        auto range = m_eventRegistery.equal_range(id);
        for (auto& it = range.first; it != range.second; ++it) {
            it->second(p);
        }

        Engine::popEvent();
    }
}

} // namespace R3

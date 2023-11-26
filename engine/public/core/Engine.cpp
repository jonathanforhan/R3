#include "Engine.hpp"
#include <chrono>

namespace R3 {

Engine::Engine()
    : m_window("R3"),
      m_renderer({
          .window = &m_window,
      }),
      m_activeScene(nullptr) {}

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

auto Engine::activeScene() -> Scene& {
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
        // engine.m_renderer.predraw();
        engine.m_activeScene->runSystems(engine.deltaTime());
        engine.m_window.update();
    }
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
    // LOG(Verbose, "Delta Time mircoseconds:", duration_cast<microseconds>(now - s_prev).count(), " -- Engine.cpp
    // line", __LINE__);
    s_prev = now;
    return dt;
}

} // namespace R3

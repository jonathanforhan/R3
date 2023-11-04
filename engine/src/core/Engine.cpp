#include "Engine.hpp"

namespace R3 {

Engine::Engine()
    : m_window("R3"),
      m_renderer() {}

auto Engine::instance() -> Engine& {
    static Engine s_instance;
    return s_instance;
}

auto Engine::addScene(const std::string& name, bool setActive) -> Scene& {
    m_scenes.try_emplace(name);
    if (setActive) {
        setActiveScene(name);
    }
    return m_scenes.at(name);
}

void Engine::removeScene(const std::string& name) {
    auto it = m_scenes.find(name);
    if (it != m_scenes.end()) {
        m_scenes.erase(it);
    }
}

} // namespace R3

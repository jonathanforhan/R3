#pragma once
#include <unordered_map>
#include "core/Renderer.hpp"
#include "core/Scene.hpp"
#include "core/Window.hpp"

namespace R3 {

class Engine final {
private:
    Engine();

public:
    Engine(const Engine&) = delete;
    void operator=(const Engine&) = delete;

    static auto instance() -> Engine&;

    // Scene Management
    auto addScene(const std::string& name, bool setActive = false) -> Scene&;
    void removeScene(const std::string& name);
    auto activeScene() -> Scene& { return *m_activeScene; }
    auto getScene(const std::string& name) -> Scene& { return m_scenes.at(name); }
    auto isActiveScene(const std::string& name) -> bool const { return &m_scenes.at(name) == m_activeScene; }
    void setActiveScene(const std::string& name) { m_activeScene = &m_scenes.at(name); }

    template <typename F>
    void gameLoop(F&& func);

private:
    Window m_window;
    Renderer m_renderer;
    std::unordered_map<std::string, Scene> m_scenes;
    Scene* m_activeScene;
};

template <typename F>
inline void Engine::gameLoop(F&& func) {
    m_window.show();
    while (!m_window.shouldClose()) {
        func();
        m_renderer.render();
        m_window.update();
    }
}

} // namespace R3
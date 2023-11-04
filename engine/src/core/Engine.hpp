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

    static void initialize() { (void)inst(); }

    // Scene Management
    static auto addScene(const std::string& name, bool setActive = false) -> Scene&;
    static void removeScene(const std::string& name);
    static auto activeScene() -> Scene&;
    static auto getScene(const std::string& name) -> Scene&;
    static auto isActiveScene(const std::string& name);
    static void setActiveScene(const std::string& name);

    static void loop();

    static auto window() -> Window& { return Engine::inst().m_window; }
    static auto renderer() -> Renderer& { return Engine::inst().m_renderer; }

private:
    static auto inst() -> Engine&;
    auto deltaTime() -> double const;

private:
    Window m_window;
    Renderer m_renderer;
    std::unordered_map<std::string, Scene> m_scenes;
    Scene* m_activeScene;
};

} // namespace R3
#pragma once
#include <unordered_map>
#include "core/Scene.hpp"
#include "render/Renderer.hpp"
#include "render/Window.hpp"

namespace R3 {

/// @brief Singleton Game-Engine class used to access the Window, Renderer, and Scenes
class Engine final {
private:
    Engine();

public:
    Engine(const Engine&) = delete;
    void operator=(const Engine&) = delete;

    ~Engine();

    /// @brief Add scene to scene-map by name
    /// @param name name of scene
    /// @param setActive activation scene on creation
    /// @return scene created
    static Scene& addScene(const std::string& name, bool setActive = false);

    /// @brief Remove scene from scene-map by name
    /// @param name name of scene
    static void removeScene(const std::string& name);

    /// @return active scene
    static Scene& activeScene();

    /// @param name scene name
    /// @return scene by name
    static Scene& getScene(const std::string& name);

    /// @param name scene name
    /// @return if scene is active as tracked by Engine
    static bool isActiveScene(const std::string& name);

    /// @brief Set the current active scene
    /// @param name scene name
    static void setActiveScene(const std::string& name);

    /// @brief Starts main game loop
    /// @warning this function will not return until the gameloop i.e. the game is over
    static void loop();

    /// @return Engine instance's Window
    static Window& window() { return Engine::inst().m_window; }

    /// @return Engine instance's Renderer
    static Renderer& renderer() { return Engine::inst().m_renderer; }

private:
    /// @brief Returns the Engine singleton instance, this will initialize the engine if not already initialized
    /// @return instance
    static Engine& inst();

    /// @return delta time between frames
    double deltaTime() const;

private:
    Window m_window;
    Renderer m_renderer;
    std::unordered_map<std::string, Scene> m_scenes;
    Scene* m_activeScene;
};

} // namespace R3
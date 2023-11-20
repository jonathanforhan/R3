#pragma once
#include <unordered_map>
#include "core/Renderer.hpp"
#include "core/Scene.hpp"
#include "core/Window.hpp"

namespace R3 {

/*!
 *  \brief Singleton Game-Engine class used to access the Window, Renderer, and Scenes
 */
class Engine final {
private:
    Engine();

public:
    Engine(const Engine&) = delete;
    void operator=(const Engine&) = delete;

    //! add scene to scene-map by name
    static Scene& addScene(const std::string& name, bool setActive = false);

    //! remove scene from scene-map by name
    static void removeScene(const std::string& name);

    //! returns the current active scene
    static Scene& activeScene();

    //! get a scene by name
    static Scene& getScene(const std::string& name);

    //! returns whether a scene is the current active scene
    static bool isActiveScene(const std::string& name);

    //! set the current active scene
    static void setActiveScene(const std::string& name);

    /*!
     * \brief starts main game loop
     * \warning this function will not return until the gameloop i.e. the game is over
     */ 
    static void loop();

    //! Window getter
    static Window& window() { return Engine::inst().m_window; }

    //! Renderer getter
    static Renderer& renderer() { return Engine::inst().m_renderer; }

private:
    //! returns the Engine singleton instance, this will initialize the engine if not already initialized
    static Engine& inst();

    //! returns the delta time between each frame / tick
    double deltaTime() const;

private:
    Window m_window;
    Renderer m_renderer;
    std::unordered_map<std::string, Scene> m_scenes;
    Scene* m_activeScene;
};

} // namespace R3
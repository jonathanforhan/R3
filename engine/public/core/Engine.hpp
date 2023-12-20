#pragma once

#include <queue>
#include <unordered_map>
#include "input/Event.hpp"
#include "render/Renderer.hpp"
#include "render/Window.hpp"

namespace R3 {

class Scene; ///< @private

/// @brief Singleton Game-Engine class used to access the Window, Renderer, and Scenes
/// all members are static and life the lifetime of the program
class Engine final {
private:
    Engine();

public:
    Engine(const Engine&) = delete;
    Engine(Engine&&) = delete;
    ~Engine();

    /// @brief Window getter
    /// @return Window
    static Window& window() { return s_instance.window; }

    /// @brief Renderer getter
    /// @return Renderer
    static Renderer& renderer() { return s_instance.renderer; }

    /// @brief Scene getter
    /// @return Scene
    static Scene* activeScene() { return s_instance.activeScene; }

    /// @brief Starts main game loop
    /// @warning this function will not return until the gameloop (i.e. the game) is over
    static void loop();

private:
    /// @brief Add scene, automatically assigns it an id
    /// @param setActive should set scene active?
    /// @return Scene
    static Scene* addScene(bool setActive = false);

    /// @brief Remove scene by uuid ie scene.id
    /// @param id uuid
    static void removeScene(uuid32 id);

private:
    /// @return delta time between frames
    static double deltaTime();

    static void dispatchEvents();

private:
    static struct Instance {
        Window window;
        Renderer renderer;
        Scene* activeScene;
        uint32 sceneCounter;
        std::unordered_map<uuid32, Scene*> scenes;
        static Engine engine;
    } s_instance;

    friend class Scene;
    friend class Entity;
};

} // namespace R3

#include "core/Engine.ipp"
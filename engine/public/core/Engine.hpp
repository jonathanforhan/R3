#pragma once

/// @file Engine.hpp
/// @brief Main Engine that controls scenes and other functionality

#include <queue>
#include <unordered_map>
#include "api/Construct.hpp"
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
    NO_COPY(Engine);
    NO_MOVE(Engine);

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

    /// @return delta time between frames
    static double deltaTime();

    /// @brief run event dispath on seperate thread
    static void dispatchEvents();

private:
    // TODO provide EngineLoader class
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
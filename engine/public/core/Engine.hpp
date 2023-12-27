#pragma once

/// @file Engine.hpp
/// @brief Main Engine that controls scenes and other functionality

#include <queue>
#include <unordered_map>
#include "api/Construct.hpp"
#include "input/Event.hpp"
#include "render/Renderer.hpp"
#include "render/Window.hpp"
#include "ui/Widget.hpp"

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

    /// @brief Free Scenes
    ~Engine();

    /// @brief Window getter
    /// @return Window
    static Window& window() { return s_window; }

    /// @brief Renderer getter
    /// @return Renderer
    static Renderer& renderer() { return s_renderer; }

    /// @brief Scene getter
    /// @return Scene
    static Scene& activeScene() { return *s_engine.m_activeScene; }

    /// @brief Starts main game loop
    /// @warning this function will not return until the gameloop (i.e. the game) is over
    static void loop();

    /// @brief Get the Engine's root widget which acts as the root of the ui tree
    /// @return Root Widget instance
    static Ref<ui::Widget> rootWidget();

private:
    /// @brief Add scene, automatically assigns it an id
    /// @param setActive should set scene active?
    /// @return Scene
    static Ref<Scene> addScene(bool setActive = false);

    /// @brief Remove scene by uuid ie scene.id
    /// @param id uuid
    static void removeScene(uuid32 id);

    /// @return delta time between frames
    static double deltaTime();

    /// @brief run event dispath on seperate thread
    static void dispatchEvents();

private:
    static Window s_window;
    static Renderer s_renderer;
    static Engine s_engine;

    uint32 m_sceneCounter;
    std::unordered_map<uuid32, Scene*> m_scenes;
    Ref<Scene> m_activeScene;
};

} // namespace R3
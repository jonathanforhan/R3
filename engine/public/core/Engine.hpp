#pragma once

#include <functional>
#include <queue>
#include <unordered_map>
#include "core/Scene.hpp"
#include "input/Event.hpp"
#include "render/Renderer.hpp"
#include "render/Window.hpp"

namespace R3 {

/// @brief Singleton Game-Engine class used to access the Window, Renderer, and Scenes
class Engine final {
private:
    Engine();

public:
    Engine(const Engine&) = delete;
    Engine(Engine&&) = delete;

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
    /// @warning this function will not return until the gameloop (i.e. the game) is over
    static void loop();

    /// @return Engine instance's Window
    static Window& window() { return Engine::inst().m_window; }

    /// @return Engine instance's Renderer
    static Renderer& renderer() { return Engine::inst().m_renderer; }

    /// @brief Push an Event onto the Event queue. This event will be called on this game tick.
    /// Events get freed from heap memory when they are dispatched. There is a static_assert
    /// on Event ensuring that the Event is trivially deconstructable so we can delete the void*
    template <typename Event, typename... Args>
    requires requires(Args&&... args) { std::is_constructible_v<typename Event::PayloadType, Args...>; }
    static constexpr void pushEvent(Args&&... args);

    static void popEvent();

    static uuid32 topEvent();

    template <typename F>
    requires EventListener<F>
    static constexpr void bindEventListener(F&& callback);

private:
    /// @brief Returns the Engine singleton instance, this will initialize the engine if not already initialized
    /// @return instance
    static Engine& inst();

    /// @return delta time between frames
    double deltaTime() const;

    void dispatchEvents();

    template <typename Event, typename... Args>
    requires requires(Args&&... args) { std::is_constructible_v<typename Event::PayloadType, Args...>; }
    void pushEventHelper(Args&&... args);

    void popEventHelper();

    // non-static allows us to call this in the engine constructor
    template <typename F>
    requires EventListener<F>
    void bindEventListenerHelper(F&& callback);

private:
    Window m_window;
    Renderer m_renderer;

    //--- Scene System
    std::unordered_map<std::string, Scene> m_scenes;
    Scene* m_activeScene;

    //--- Event System
    std::queue<std::pair<void*, usize>> m_eventQueue; // tracks event (as void*) and the payload size
    std::vector<uint8> m_eventArena;                  // memory pool for allocations when pushing events
    std::unordered_multimap<uuid32, std::function<void(void*)>> m_eventRegistery; // mapping of id to callback
};

} // namespace R3

#include "core/Engine.ipp"
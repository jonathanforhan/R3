#pragma once

#include "api/Class.hpp"

namespace R3 {

class Engine final {
private:
    R3_CTOR_DEFAULT(Engine);

public:
    /// Get Global Game Window
    class Window& Window() noexcept { return *m_window; }
    /// Get Global Engine RenderContext
    class IRenderContext& RenderContext() noexcept { return *m_ctx; }
    /// Get Global Engine RenderContext cast as T
    template <typename T>
    T& RenderContext() noexcept {
        return *static_cast<T*>(m_ctx);
    }
    /// Get Global EventHandler
    class EventHandler& EventHandler() noexcept { return *m_eventHandler; }
    /// Get Global ResourceManager
    class ResourceManager& ResourceManager() noexcept { return *m_resourceManager; }
    /// Get Global WorldState
    class World& World() noexcept { return *m_world; }

    int run();

private:
    double deltaTime();

private:
    class Window* m_window                   = nullptr;
    class IRenderContext* m_ctx              = nullptr;
    class EventHandler* m_eventHandler       = nullptr;
    class ResourceManager* m_resourceManager = nullptr;
    class World* m_world                     = nullptr;
    bool m_running                           = false;

private:
    friend struct GEngine;
    friend struct GWindow;
    friend struct GRenderContext;
    friend struct GEventHandler;
    friend struct GResourceManager;
    friend struct GWorld;
};

/// @brief Engine singleton instance.
struct GEngine {
    Engine* operator->() noexcept {
        static Engine instance;
        return &instance;
    }
};

struct GEventHandler {
    class EventHandler* operator->() noexcept { return &(GEngine()->EventHandler()); }
};

struct GResourceManager {
    class ResourceManager* operator->() noexcept { return &(GEngine()->ResourceManager()); }
};

struct GWindow {
    class Window* operator->() noexcept { return &(GEngine()->Window()); }
};

struct GRenderContext {
    class IRenderContext* operator->() noexcept { return &(GEngine()->RenderContext()); }
};

struct GWorld {
    class World* operator->() noexcept { return &(GEngine()->World()); }
};

} // namespace R3

#include "EventHandler.hpp"
#include "core/ResourceManager.hpp"
#include "core/World.hpp"
#include "render/RenderContext.hpp"
#include "render/Window.hpp"

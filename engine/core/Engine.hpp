#pragma once

#include "engine/api/Api.hpp"

#include "editor/Editor.hpp"
#include "engine/api/Api.hpp"
#include "engine/api/Class.hpp"

#include "engine/render/vulkan/vulkan-Renderer.hpp"

namespace R3 {

class R3_API Engine final {
private:
    R3_CTOR_DEFAULT(Engine);
    R3_COPY_DELETE(Engine);
    R3_MOVE_DELETE(Engine);

public:
    /// Get Global Game Window
    class R3_API Window& Window() noexcept { return *m_window; }
    /// Get Global Engine RenderContext
    class R3_API IRenderContext& RenderContext() noexcept { return *m_ctx; }
    /// Get Global Engine RenderContext cast as T
    template <typename T>
    T& RenderContext() noexcept {
        return *static_cast<T*>(m_ctx);
    }
    /// Get Global EventHandler
    class R3_API EventHandler& EventHandler() noexcept { return *m_eventHandler; }
    /// Get Global ResourceManager
    class R3_API ResourceManager& ResourceManager() noexcept { return *m_resourceManager; }
    /// Get Global WorldState
    class R3_API World& World() noexcept { return *m_world; }

    void update();

private:
    void initialize();

    void shutdown() noexcept;

    double deltaTime();

    void resetState();

private:
    class R3_API Window* m_window                   = nullptr;
    class R3_API IRenderContext* m_ctx              = nullptr;
    class R3_API EventHandler* m_eventHandler       = nullptr;
    class R3_API ResourceManager* m_resourceManager = nullptr;
    class R3_API World* m_world                     = nullptr;
    // hidden
    class R3_API vulkan::Renderer* m_renderer = nullptr;

#if R3_EDITOR
    class R3_API IEditor* m_editor = nullptr;
#endif

    bool m_running = false;

private:
    friend struct GEngine;
    friend struct GWindow;
    friend struct GRenderContext;
    friend struct GEventHandler;
    friend struct GResourceManager;
    friend struct GWorld;
    friend class Application;
    friend void vulkan::Renderer::draw();
};

/// @brief Engine singleton instance.
struct R3_API GEngine {
    Engine* operator->() noexcept;
};

struct GEventHandler {
    class R3_API EventHandler* operator->() noexcept { return &(GEngine()->EventHandler()); }
};

struct GResourceManager {
    class R3_API ResourceManager* operator->() noexcept { return &(GEngine()->ResourceManager()); }
};

struct GWindow {
    class R3_API Window* operator->() noexcept { return &(GEngine()->Window()); }
};

struct GRenderContext {
    class R3_API IRenderContext* operator->() noexcept { return &(GEngine()->RenderContext()); }
};

struct GWorld {
    class R3_API World* operator->() noexcept { return &(GEngine()->World()); }
};

} // namespace R3

#include "EventHandler.hpp"
#include "engine/core/ResourceManager.hpp"
#include "engine/core/World.hpp"
#include "engine/render/RenderContext.hpp"
#include "engine/render/Window.hpp"

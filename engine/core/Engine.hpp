#pragma once

#include "engine/api/Api.hpp"

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
    const class R3_API Window& Window() const noexcept { return *m_window; }
    /// Get Global Engine RenderContext
    class R3_API IRenderContext& RenderContext() noexcept { return *m_ctx; }
    const class R3_API IRenderContext& RenderContext() const noexcept { return *m_ctx; }
    /// Get Global Engine RenderContext cast as T
    template <typename T>
    T& RenderContext() noexcept {
        return *static_cast<T*>(m_ctx);
    }
    template <typename T>
    const T& RenderContext() const noexcept {
        return *static_cast<const T*>(m_ctx);
    }
    /// Get Global EventHandler
    class R3_API EventHandler& EventHandler() noexcept { return *m_eventHandler; }
    const class R3_API EventHandler& EventHandler() const noexcept { return *m_eventHandler; }
    /// Get Global ResourceManager
    class R3_API ResourceManager& ResourceManager() noexcept { return *m_resourceManager; }
    const class R3_API ResourceManager& ResourceManager() const noexcept { return *m_resourceManager; }
    /// Get Global WorldState
    class R3_API World& World() noexcept { return *m_world; }
    const class R3_API World& World() const noexcept { return *m_world; }
    /// Get Global Editor
    /// @note May be nullptr if in distribution build
    class R3_API IEditor* Editor() noexcept { return m_editor; }

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
    class R3_API IEditor* m_editor                  = nullptr;

    // hidden
    class R3_API vulkan::Renderer* m_renderer = nullptr;

    bool m_running = false;

private:
    friend struct GEngine;
    friend class Application;
};

/// @brief Engine singleton instance.
struct R3_API GEngine {
    Engine* operator->() const noexcept;
    operator Engine*() noexcept { return operator->(); }
    operator const Engine*() const noexcept { return operator->(); }
};

template <typename T, T& (Engine::*Fn)() noexcept, const T& (Engine::*CFn)() const noexcept>
struct IGlobalAccessor {
    T* operator->() noexcept { return &((GEngine()->*Fn)()); }
    const T* operator->() const noexcept { return &((GEngine()->*CFn)()); }
    operator T&() noexcept { return *operator->(); }
    operator const T&() const noexcept { return *operator->(); }
};

using GEventHandler    = IGlobalAccessor<class EventHandler, &Engine::EventHandler, &Engine::EventHandler>;
using GResourceManager = IGlobalAccessor<class ResourceManager, &Engine::ResourceManager, &Engine::ResourceManager>;
using GWindow          = IGlobalAccessor<class Window, &Engine::Window, &Engine::Window>;
using GRenderContext   = IGlobalAccessor<class IRenderContext, &Engine::RenderContext, &Engine::RenderContext>;
using GWorld           = IGlobalAccessor<class World, &Engine::World, &Engine::World>;

#if R3_EDITOR
struct GEditor {
    class R3_API IEditor* operator->() noexcept { return GEngine()->Editor(); }
    operator class R3_API IEditor &() noexcept { return *operator->(); }
};
#endif

} // namespace R3

#include "engine/core/EventHandler.hpp"
#include "engine/core/ResourceManager.hpp"
#include "engine/core/World.hpp"
#include "engine/editor/Editor.hpp"
#include "engine/render/RenderContext.hpp"
#include "engine/render/Window.hpp"

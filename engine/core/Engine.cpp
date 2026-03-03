#include "Engine.hpp"

#include <chrono>
#include <new>
#include "EventHandler.hpp"
#include "core/ResourceManager.hpp"
#include "core/World.hpp"
#include "engine/editor/Editor.hpp"
#include "input/InputCodes.hpp"
#include "input/InputEvents.hpp"
#include "render/Window.hpp"
#include "render/vulkan/vulkan-RenderContext.hpp"
#include "render/vulkan/vulkan-Renderer.hpp"

namespace R3 {

void Engine::update() {
    const double dt = deltaTime();
    GWorld()->update(dt);

    bool uiFocused = false;
    if (!m_window->isMinimized()) {
#if R3_EDITOR
        m_editor->recordFrame(dt);
        uiFocused = m_editor->uiFocused();
#endif
        m_renderer->acquire();
        m_renderer->update();
        m_renderer->render();
        m_renderer->present();
    }

    GEventHandler()->dispatchEvents();

    m_window->update(uiFocused);

    GEventHandler()->emplace("frame-done");
}

void Engine::initialize() {
    m_eventHandler    = new class EventHandler;
    m_resourceManager = new class ResourceManager;
    m_window          = new class Window;
    m_ctx             = new vulkan::RenderContext(*m_window);
    m_world           = new class World;
    m_renderer        = new vulkan::Renderer(*m_window, *static_cast<vulkan::RenderContext*>(m_ctx));

    /* Add callback to show window once the first frame is rendered, this prevents white screen */
    m_eventHandler->bindEventListener("frame-done", [this] noexcept {
        m_window->show();
        return true; // remove after first call
    });
}

void Engine::shutdown() noexcept {
    vulkan::RenderContext& ctx = *static_cast<vulkan::RenderContext*>(m_ctx);
    ctx.waitIdle();

    resetState();

#if R3_EDITOR
    if (m_editor) {
        delete m_editor;
    }
#endif

    delete m_renderer;
    delete m_world;
    delete m_ctx;
    delete m_window;
    delete m_eventHandler;
    delete m_resourceManager;
}

double Engine::deltaTime() {
    using namespace std::chrono;

    static auto s_prev = system_clock::now();

    const auto curr = system_clock::now();
    const double dt = duration<double>(curr - s_prev).count();
    // ^^ seconds

    s_prev = curr;
    return dt;
}

void Engine::resetState() {
    if (m_world) {
        m_world->registry().clear();
    }

    if (m_resourceManager) {
        m_resourceManager->clear();
    }
}

Engine* GEngine::operator->() noexcept {
    static Engine instance;
    return &instance;
}

} // namespace R3

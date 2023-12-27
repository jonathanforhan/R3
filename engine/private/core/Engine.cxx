#include "core/Engine.hpp"

#include <chrono>
#include "api/Profile.hxx"
#include "core/Scene.hpp"
#include "input/WindowEvent.hpp"
#include "render/ResourceManager.hxx"
#include "systems/InputSystem.hpp"
#include "ui/RootWidget.hxx"
#include "ui/UserInterface.hxx"

#if R3_BUILD_EDITOR and R3_VULKAN
#include "editor/Editor.hxx"
#endif // R3_BUILD_EDITOR

namespace R3 {

Window Engine::s_window({.title = "R3"});
Renderer Engine::s_renderer({.window = s_window});
Engine Engine::s_engine = {};

Engine::Engine()
    : m_activeScene(addScene(true)) {}

Engine::~Engine() {
    for (auto& scene : s_engine.m_scenes) {
        delete scene.second;
    }
}

void Engine::loop() {
    ui::UserInterface ui;
#if R3_BUILD_EDITOR and R3_VULKAN
    editor::Editor editor;
#endif

    s_window.show();

    while (!s_window.shouldClose() || !s_engine.m_activeScene->m_eventQueue.empty()) {
        R3_PROFILE(FRAME, "MainThread");

        double dt = deltaTime();

        R3_PROFILE(PUSH, "Events");
        dispatchEvents();
        R3_PROFILE(POP);

        R3_PROFILE(PUSH, "Systems");
        s_engine.m_activeScene->runSystems(dt);
        R3_PROFILE(POP);

        s_renderer.setView(s_engine.m_activeScene->view());
        s_renderer.setProjection(s_engine.m_activeScene->projection());

        R3_PROFILE(PUSH, "Render");
        ui::UserInterface::beginFrame();
#if R3_BUILD_EDITOR and R3_VULKAN
        editor.draw();
#endif
        ui::UserInterface::endFrame();

        s_renderer.render();
        R3_PROFILE(POP);

        s_window.update();
    }

    // sync
    s_renderer.waitIdle();

    // destroy ECS (must do explicitly due to static object lifetimes ie the GlobalResourceManager)
    for (auto& scene : s_engine.m_scenes) {
        scene.second->m_registry.clear();
    }
}

Ref<ui::Widget> Engine::rootWidget() {
    static ui::RootWidget root;
    return &root;
}

Ref<Scene> Engine::addScene(bool setActive) {
    auto* scene = new Scene(s_engine.m_sceneCounter++);
    s_engine.m_scenes.emplace(scene->id, scene);

    if (setActive) {
        s_engine.m_activeScene = scene;
    }

    // always bind resize listener and input system
    Scene::bindEventListener([](const WindowResizeEvent&) { s_renderer.resize(); });
    Scene::addSystem<InputSystem>();

    return scene;
}

void Engine::removeScene(uuid32 id) {
    auto it = s_engine.m_scenes.find(id);
    if (it != s_engine.m_scenes.end()) {
        delete it->second;
        s_engine.m_scenes.erase(it);
    }
}

double Engine::deltaTime() {
    using namespace std::chrono;

    static auto s_prev = system_clock::now();
    auto now = system_clock::now();
    double dt = duration<double>(now - s_prev).count();
    s_prev = now;
    return dt;
}

void Engine::dispatchEvents() {
    while (!s_engine.m_activeScene->m_eventQueue.empty()) {
        void* const p = s_engine.m_activeScene->m_eventQueue.front().data();
        const uuid32 id = *(uuid32*)p;

        auto range = s_engine.m_activeScene->m_eventRegistery.equal_range(id);
        for (auto& it = range.first; it != range.second; ++it) {
            it->second(p);
        }

        Scene::popEvent();
    }
}

} // namespace R3

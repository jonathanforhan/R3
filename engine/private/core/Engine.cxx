#include "core/Engine.hpp"

#include <chrono>
#include "core/Scene.hpp"
#include "input/WindowEvent.hpp"
#include "systems/InputSystem.hpp"

#if R3_BUILD_EDITOR
#include "Editor.hpp"
#endif

namespace R3 {

//--- Static Initialization
Engine::Instance Engine::s_instance = {
    .window = Window({.title = "R3"}),
    .renderer = Renderer({.window = s_instance.window}),
    .activeScene = nullptr,
    .sceneCounter = 0,
    .scenes = {},
};
Engine Engine::Instance::engine = {};

Engine::Engine() {
    addScene(true);
}

Engine::~Engine() {
    for (auto& scene : s_instance.scenes) {
        delete scene.second;
    }
}

void Engine::loop() {
#if R3_BUILD_EDITOR
    editor::Editor editor;
    s_instance.renderer.editorDrawCallback = [&](Ref<const CommandBuffer> cmd) { editor.drawUI(cmd); };
#endif

    s_instance.window.show();

    while (!s_instance.window.shouldClose() || !s_instance.activeScene->m_eventQueue.empty()) {
#if R3_BUILD_EDITOR
        editor.newFrame();
#endif

        double dt = deltaTime();
        dispatchEvents();
        s_instance.activeScene->runSystems(dt);
        s_instance.renderer.setView(s_instance.activeScene->view());
        s_instance.renderer.setProjection(s_instance.activeScene->projection());
        s_instance.renderer.render();
        s_instance.window.update();
    }

    // sync
    s_instance.renderer.waitIdle();

    // destroy ECS (must do explicitly due to static object lifetimes ie the GlobalResourceManager)
    for (auto& scene : s_instance.scenes) {
        scene.second->m_registry.clear();
    }
}

Scene* Engine::addScene(bool setActive) {
    auto* scene = new Scene(s_instance.sceneCounter++);
    s_instance.scenes.emplace(scene->id, scene);

    if (setActive) {
        s_instance.activeScene = scene;
    }

    // always bind resize listener and input system
    Scene::bindEventListener([](const WindowResizeEvent&) { s_instance.renderer.resize(); });
    Scene::addSystem<InputSystem>();

    return scene;
}

void Engine::removeScene(uuid32 id) {
    auto it = s_instance.scenes.find(id);
    if (it != s_instance.scenes.end()) {
        delete it->second;
        s_instance.scenes.erase(it);
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
    while (!s_instance.activeScene->m_eventQueue.empty()) {
        void* const p = s_instance.activeScene->m_eventQueue.front().data();
        const uuid32 id = *(uuid32*)p;

        auto range = s_instance.activeScene->m_eventRegistery.equal_range(id);
        for (auto& it = range.first; it != range.second; ++it) {
            it->second(p);
        }

        Scene::popEvent();
    }
}

} // namespace R3

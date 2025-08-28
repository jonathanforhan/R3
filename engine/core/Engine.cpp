#include "Engine.hpp"

#include <chrono>
#include "EventHandler.hpp"
#include "components/LightComponent.hpp"
#include "components/TransformComponent.hpp"
#include "core/Entity.hpp"
#include "core/ResourceManager.hpp"
#include "core/World.hpp"
#include "editor/Editor.hpp"
#include "media/ModelLoader.hpp"
#include "render/Window.hpp"
#include "render/vulkan/vulkan-RenderContext.hpp"
#include "render/vulkan/vulkan-Renderer.hpp"
#include "systems/TransformSystem.hpp"

namespace R3 {

static void TEST_FUNCTION() {
    World()->addSystem<TransformSystem>();

    // ModelLoader().glTFLoad("assets/glTF-samples/Models/DamagedHelmet/glTF/DamagedHelmet.gltf");
    Entity helmet = ModelLoader().glTFLoad("assets/glTF-samples/Models/DamagedHelmet/glTF-Binary/DamagedHelmet.glb");

    Entity chess = ModelLoader().glTFLoad("assets/glTF-samples/Models/ABeautifulGame/glTF/ABeautifulGame.gltf");

    Entity light = World()->registry().create();
    World()->registry().emplace<LightComponent>(light,
                                                LightComponent{
                                                    .position  = fvec3(2.0f, 4.0f, 0.0f),
                                                    .color     = fvec3(1.0f),
                                                    .intensity = 100.0f,
                                                });

    auto& t       = World()->registry().get<TransformComponent>(helmet);
    t.transform() = glm::translate(t.transform(), fvec3(0.5f, 0.0f, 0.0f));
    t.transform() = glm::scale(t.transform(), fvec3(0.2f));
}

double EngineSingleton::deltaTime() {
    using namespace std::chrono;

    static auto s_prev = system_clock::now();

    const auto curr = system_clock::now();
    const double dt = duration<double>(curr - s_prev).count();

    s_prev = curr;
    return dt;
}

int EngineSingleton::run() {
    if (m_running) {
        return -1;
    } else {
        m_running = true;
    }

    Window window;

    vulkan::RenderContext ctx{window};
    m_ctx = &ctx;

    TEST_FUNCTION();

    vulkan::Renderer renderer{window, ctx};
    {
#if R3_EDITOR
        Editor editor(window, ctx);
#endif
        while (!window.shouldClose()) {
            const double dt = deltaTime();

            World()->update(dt);
            window.update();

            EventHandler()->dispatchEvents();

            if (!window.isMinimized()) {
#if R3_EDITOR
                editor.recordInterfaceFrame(dt);
#endif
                renderer.draw(dt);
            }

            EventHandler()->emplace("frame-done");
        }

        ctx.waitIdle();
    }

    World()->registry().clear();
    ResourceManager()->clear();

    return 0;
}

} // namespace R3

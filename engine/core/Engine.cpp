#include "Engine.hpp"

#include <chrono>
#include <ratio>
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
    GWorld()->addSystem<TransformSystem>();

    // ModelLoader().glTFLoad("assets/glTF-samples/Models/DamagedHelmet/glTF/DamagedHelmet.gltf");
    // Entity helmet = ModelLoader().glTFLoad("assets/glTF-samples/Models/DamagedHelmet/glTF-Binary/DamagedHelmet.glb");
    Entity chess = ModelLoader().glTFLoad("assets/glTF-samples/Models/ABeautifulGame/glTF/ABeautifulGame.gltf");
    // Entity car = ModelLoader().glTFLoad("assets/glTF-samples/Models/CarConcept/glTF/CarConcept.gltf");
    // Entity city = ModelLoader().glTFLoad("assets/glTF-samples/Models/VirtualCity/glTF-Binary/VirtualCity.glb");
    Entity sponza = ModelLoader().glTFLoad("assets/glTF-samples/Models/Sponza/glTF/Sponza.gltf");
    // Entity lamp = ModelLoader().glTFLoad("assets/glTF-samples/Models/StainedGlassLamp/glTF/StainedGlassLamp.gltf");

    Entity light = GWorld()->registry().create();
    GWorld()->registry().emplace<LightComponent>(light,
                                                 LightComponent{
                                                     .position  = fvec3(0.0f, 2.0f, 0.0f),
                                                     .color     = fvec3(1.0f),
                                                     .intensity = 5.0f,
                                                 });

    auto& t = GWorld()->registry().get<TransformComponent>(chess).transform();
    t       = glm::translate(t, fvec3(0.0f, 1.0f, 0.0f));

    /*
    auto& t       = GWorld()->registry().get<TransformComponent>(helmet);
    t.transform() = glm::translate(t.transform(), fvec3(0.8f, 0.0f, 0.0f));
    t.transform() = glm::scale(t.transform(), fvec3(0.2f));
    */
}

double Engine::deltaTime() {
    using namespace std::chrono;

    static auto s_prev = system_clock::now();

    const auto curr = system_clock::now();
    const double dt = duration<double, std::milli>(curr - s_prev).count();
    // ^^ milliseconds

    s_prev = curr;
    return dt;
}

int Engine::run() {
    if (m_running) {
        return -1;
    } else {
        m_running = true;
    }

    class Window window;
    vulkan::RenderContext ctx{window};
    class EventHandler eventHandler;
    class ResourceManager resourceManager;
    class World world;

    m_window          = &window;
    m_ctx             = &ctx;
    m_eventHandler    = &eventHandler;
    m_resourceManager = &resourceManager;
    m_world           = &world;

    TEST_FUNCTION();

    vulkan::Renderer renderer{window, ctx};
    {
#if R3_EDITOR
        Editor editor(window, ctx);
#endif
        while (!window.shouldClose()) {
            const double dt = deltaTime();

            GWorld()->update(dt);

            GEventHandler()->dispatchEvents();

            if (!window.isMinimized()) {
#if R3_EDITOR
                editor.recordInterfaceFrame(dt);
#endif
                renderer.draw();
            }

            window.update();

            GEventHandler()->emplace("frame-done");
        }

        ctx.waitIdle();
    }

    GWorld()->registry().clear();
    GResourceManager()->clear();

    m_window          = nullptr;
    m_ctx             = nullptr;
    m_eventHandler    = nullptr;
    m_resourceManager = nullptr;
    m_world           = nullptr;

    return 0;
}

} // namespace R3

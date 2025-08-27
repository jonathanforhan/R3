#include "Engine.hpp"

#include <chrono>
#include "EventHandler.hpp"
#include "core/Entity.hpp"
#include "core/EventHandler.hpp"
#include "core/ResourceManager.hpp"
#include "core/World.hpp"
#include "media/ModelLoader.hpp"
#include "render/Window.hpp"
#include "render/vulkan/vulkan-RenderContext.hpp"
#include "render/vulkan/vulkan-Renderer.hpp"

namespace R3 {

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

    //--- Window
    Window window;

    //--- Render Context
    //    - instance
    //    - surface
    //    - physical device
    //    - logical device
    //    - queues
    //    - command buffers/pools
    //      - each collection of command buffers shares a command pool
    //    - sync objects
    //    - descriptor set layouts
    vulkan::RenderContext ctx{window};
    m_ctx = &ctx;

    const char* modelPath = "assets/glTF-samples/Models/DamagedHelmet/glTF-Binary/DamagedHelmet.glb";
    (void)ModelLoader().glTFLoad(modelPath);

    vulkan::Renderer renderer{window, ctx};

    window.show();

    while (!window.shouldClose()) {
        const double dt = deltaTime();

        World()->update(dt);

        window.update();

        renderer.render(dt);

        EventHandler()->dispatchEvents();
        EventHandler()->emplace("frame-done");
    }

    ctx.waitIdle();

    World()->registry().clear();
    ResourceManager()->clear();

    return 0;
}

} // namespace R3

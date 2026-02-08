#include <future>
#include <engine/api/Api.hpp>
#include <engine/components/LightComponent.hpp>
#include <engine/core/Engine.hpp>
#include <engine/core/Entity.hpp>
#include <engine/core/EventHandler.hpp>
#include <engine/core/Events.hpp>
#include <engine/core/Log.hpp>
#include <engine/core/World.hpp>
#include <engine/input/InputCodes.hpp>
#include <engine/input/InputEvents.hpp>
#include <engine/media/ModelLoader.hpp>
#include <engine/render/Window.hpp>
#include <engine/systems/TransformSystem.hpp>

static R3::Entity light = entt::null;

static void moduleMain() {
    using namespace R3;

    GWorld()->addSystem<R3::TransformSystem>();

    ModelLoader loader;

#if 1
    // auto futhelmet = loader.glTFLoadAsync("assets/glTF-samples/Models/DamagedHelmet/glTF-Binary/DamagedHelmet.glb");
    auto futchess  = loader.glTFLoadAsync("assets/glTF-samples/Models/ABeautifulGame/glTF/ABeautifulGame.gltf");
    auto futsponza = loader.glTFLoadAsync("assets/glTF-samples/Models/Sponza/glTF/Sponza.gltf");

    // auto helmet = futhelmet.get();
    auto chess  = futchess.get();
    auto sponza = futsponza.get();

    {
        auto& t = GWorld()->registry().get<TransformComponent>(chess).transform();
        t       = glm::translate(t, fvec3(0.0f, 1.0f, 0.0f));
        t       = glm::scale(t, fvec3(3.0f));
    }
    {
        auto& t = GWorld()->registry().get<TransformComponent>(sponza).transform();
        // t       = glm::translate(t, fvec3(2.0f, 1.0f, 0.0f));
        // t       = glm::scale(t, fvec3(0.25));
    }
#endif

    // loader.glTFLoad("assets/glTF-samples/Models/DamagedHelmet/glTF/DamagedHelmet.gltf");
    // Entity helmet = loader.glTFLoad("assets/glTF-samples/Models/DamagedHelmet/glTF-Binary/DamagedHelmet.glb");
    // Entity chess  = loader.glTFLoad("assets/glTF-samples/Models/ABeautifulGame/glTF/ABeautifulGame.gltf");
    // Entity car = loader.glTFLoad("assets/glTF-samples/Models/CarConcept/glTF/CarConcept.gltf");
    // Entity city = loader.glTFLoad("assets/glTF-samples/Models/VirtualCity/glTF-Binary/VirtualCity.glb");
    // Entity sponza = loader.glTFLoad("assets/glTF-samples/Models/Sponza/glTF/Sponza.gltf");
    // Entity lamp = loader.glTFLoad("assets/glTF-samples/Models/StainedGlassLamp/glTF/StainedGlassLamp.gltf");

    light = GWorld()->registry().create();

    GWorld()->registry().emplace<LightComponent>(light) = {
        .position  = fvec3(-2.0f, 4.0f, -4.0f),
        .color     = fvec3(1.0f),
        .intensity = 25.0f,
    };
}

R3_MODULE void* Entry() {
    moduleMain();
    return nullptr;
}

R3_MODULE void Exit(void*) {}

R3_MODULE void Loop() {
    using namespace R3;

    if (GWindow()->keyPressed(Key::I)) {
        GWorld()->registry().get<LightComponent>(light).position.z += 0.005f;
    }
    if (GWindow()->keyPressed(Key::K)) {
        GWorld()->registry().get<LightComponent>(light).position.z -= 0.005f;
    }
    if (GWindow()->keyPressed(Key::J)) {
        GWorld()->registry().get<LightComponent>(light).position.x += 0.005f;
    }
    if (GWindow()->keyPressed(Key::L)) {
        GWorld()->registry().get<LightComponent>(light).position.x -= 0.005f;
    }
    if (GWindow()->keyPressed(Key::U)) {
        GWorld()->registry().get<LightComponent>(light).position.y += 0.005f;
    }
    if (GWindow()->keyPressed(Key::O)) {
        GWorld()->registry().get<LightComponent>(light).position.y -= 0.005f;
    }
}

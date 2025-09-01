#include <future>
#include <engine/api/Api.hpp>
#include <engine/components/LightComponent.hpp>
#include <engine/core/Engine.hpp>
#include <engine/core/Entity.hpp>
#include <engine/core/Log.hpp>
#include <engine/core/World.hpp>
#include <engine/media/ModelLoader.hpp>
#include <engine/systems/TransformSystem.hpp>

static void moduleMain() {
    using namespace R3;

    GWorld()->addSystem<R3::TransformSystem>();

    ModelLoader loader;

    auto futhelmet = loader.glTFLoadAsync("assets/glTF-samples/Models/DamagedHelmet/glTF-Binary/DamagedHelmet.glb");
    auto futchess  = loader.glTFLoadAsync("assets/glTF-samples/Models/ABeautifulGame/glTF/ABeautifulGame.gltf");
    auto futsponza = loader.glTFLoadAsync("assets/glTF-samples/Models/Sponza/glTF/Sponza.gltf");

    auto helmet = futhelmet.get();
    auto chess  = futchess.get();
    auto sponza = futsponza.get();

    // loader.glTFLoad("assets/glTF-samples/Models/DamagedHelmet/glTF/DamagedHelmet.gltf");
    // Entity helmet = loader.glTFLoad("assets/glTF-samples/Models/DamagedHelmet/glTF-Binary/DamagedHelmet.glb");
    // Entity chess  = loader.glTFLoad("assets/glTF-samples/Models/ABeautifulGame/glTF/ABeautifulGame.gltf");
    // Entity car = loader.glTFLoad("assets/glTF-samples/Models/CarConcept/glTF/CarConcept.gltf");
    // Entity city = loader.glTFLoad("assets/glTF-samples/Models/VirtualCity/glTF-Binary/VirtualCity.glb");
    // Entity sponza = loader.glTFLoad("assets/glTF-samples/Models/Sponza/glTF/Sponza.gltf");
    // Entity lamp = loader.glTFLoad("assets/glTF-samples/Models/StainedGlassLamp/glTF/StainedGlassLamp.gltf");

    Entity light = GWorld()->registry().create();
    GWorld()->registry().emplace<LightComponent>(light,
                                                 LightComponent{
                                                     .position  = fvec3(0.0f, 2.0f, 0.0f),
                                                     .color     = fvec3(1.0f),
                                                     .intensity = 5.0f,
                                                 });

    {
        auto& t = GWorld()->registry().get<TransformComponent>(chess).transform();
        t       = glm::translate(t, fvec3(0.0f, 1.0f, 0.0f));
    }
    {
        auto& t = GWorld()->registry().get<TransformComponent>(helmet).transform();
        t       = glm::translate(t, fvec3(2.0f, 1.0f, 0.0f));
        t       = glm::scale(t, fvec3(0.25));
    }
}

R3_MODULE void* Entry() {
    moduleMain();
    return nullptr;
}

R3_MODULE void Exit(void*) {}

R3_MODULE void Loop() {}

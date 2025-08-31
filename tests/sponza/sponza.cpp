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

    // ModelLoader().glTFLoad("assets/glTF-samples/Models/DamagedHelmet/glTF/DamagedHelmet.gltf");
    Entity helmet = ModelLoader().glTFLoad("assets/glTF-samples/Models/DamagedHelmet/glTF-Binary/DamagedHelmet.glb");
    // Entity chess  = ModelLoader().glTFLoad("assets/glTF-samples/Models/ABeautifulGame/glTF/ABeautifulGame.gltf");
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

    {
        // auto& t = GWorld()->registry().get<TransformComponent>(chess).transform();
        // t       = glm::translate(t, fvec3(0.0f, 1.0f, 0.0f));
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

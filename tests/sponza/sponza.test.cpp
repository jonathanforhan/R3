#include <R3>
#include <R3_core>
#include <R3_input>
#include "components/CameraComponent.hpp"
#include "components/EditorComponent.hpp"
#include "components/LightComponent.hpp"
#include "components/ModelComponent.hpp"

using namespace R3;

extern "C" {

R3_DLL void* Entry() {
    CurrentScene = new Scene(HASH32("Sponza"), "Sponza");
    return CurrentScene;
}

R3_DLL void Exit(void* scene_) {
    auto* scene = ((Scene*)scene_);
    scene->clearRegistry();
    delete scene;
}

R3_DLL void Run() {
    try {
        //--- Camera
        Entity::create<Entity>().emplace<CameraComponent>().setActive(true);

        {
            auto& light1 = Entity::create<Entity>();
            auto& lc = light1.emplace<LightComponent>();
            lc.intensity = 10.0f;
            lc.position = vec3(0, 4, 0);
            light1.emplace<EditorComponent>().name = "Light";
        }

        {
            auto& sponza = Entity::create<Entity>();
            sponza.emplace<ModelComponent>("assets/glTF/Models/Sponza/glTF/Sponza.gltf");
            sponza.emplace<EditorComponent>().name = "Sponza";
            auto& t = sponza.get<TransformComponent>();
            t = glm::translate(t, vec3(0, -2, 0));
            t = glm::scale(t, vec3(0.01f));
        }

        {
            auto& peter = Entity::create<Entity>();
            auto& c(peter.emplace<ModelComponent>(
                "assets/peter_griffin.glb", "spirv/test.vert.spv", "spirv/test.frag.spv"));
            c.animation.running = true;
            peter.emplace<EditorComponent>().name = "Peter";
            auto& t = peter.get<TransformComponent>();
        }
    } catch (std::exception const& e) {
        LOG(Error, e.what());
    }
}

} // extern "C"
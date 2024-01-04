#include <R3>
#include <R3_components>
#include <R3_core>
#include <R3_input>

using namespace R3;

extern "C" {

R3_DLL void* Entry() {
    CurrentScene = new Scene(HASH32("Default"), "Default");
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

#if 1
        auto& helmet = Entity::create<Entity>();
        helmet.emplace<ModelComponent>("assets/DamagedHelmet/glTF-Binary/DamagedHelmet.glb");
        helmet.emplace<EditorComponent>("Helmet");
        auto& htransform = helmet.get<TransformComponent>();
        htransform = glm::translate(htransform, vec3(3.0f, 0, 0));

        auto& duck = Entity::create<Entity>();
        duck.emplace<ModelComponent>("assets/Duck/glTF-Binary/Duck.glb");
        duck.emplace<EditorComponent>("Duck");
        auto& transform = duck.get<TransformComponent>();
        transform = mat4(0.01f);
        transform = glm::translate(transform, vec3(-3, -3, -3));
#else
        auto& entity = Entity::create<Entity>();
        entity.emplace<ModelComponent>("assets/Sponza/glTF/Sponza.gltf");
        auto& transform = entity.get<TransformComponent>();
        transform = glm::translate(transform, vec3(0, -2, 0));
#endif

    } catch (std::exception const& e) {
        LOG(Error, e.what());
    }
}

} // extern "C"
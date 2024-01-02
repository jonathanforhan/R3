#include <R3>
#include <R3_components>
#include <R3_core>
#include <R3_input>

using namespace R3;

extern "C" {

R3_DLL void* Entry() {
    static Scene scene = Scene(HASH32("Default"), "Default");
    CurrentScene = &scene;
    return CurrentScene;
}

R3_DLL void Run() {
    try {
        //--- Camera
        auto& cameraEntity = Entity::create<Entity>();
        auto& editorCamera = cameraEntity.emplace<CameraComponent>();
        editorCamera.setActive(true);

        auto& red = Entity::create<Entity>().emplace<LightComponent>();
        red.position = vec3(0, 1, -1);
        red.color = vec3(1, 1, 1);
        red.intensity = 1.0f;

        auto& green = Entity::create<Entity>().emplace<LightComponent>();
        green.position = vec3(0, 1, 0);
        green.color = vec3(1, 1, 1);
        green.intensity = 1.0f;

        auto& blue = Entity::create<Entity>().emplace<LightComponent>();
        blue.position = vec3(0, 1, 1);
        blue.color = vec3(1, 1, 1);
        blue.intensity = 1.0f;

#if 0
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
        transform = glm::translate(transform, vec3(0, -1, 3.0f));
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
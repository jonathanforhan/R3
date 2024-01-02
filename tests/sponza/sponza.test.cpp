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
        cameraEntity.emplace<EditorComponent>("Editor Camera");
        auto& editorCamera = cameraEntity.emplace<CameraComponent>();
        editorCamera.setActive(true);
        editorCamera.setPosition(vec3(0, 0, -3));

        auto& red = Entity::create<Entity>().emplace<LightComponent>();
        red.position = vec3(2, 1, 0);
        red.color = vec3(1, 0, 0);
        red.intensity = 0.8f;

        auto& green = Entity::create<Entity>().emplace<LightComponent>();
        green.position = vec3(-2, 1, 0);
        green.color = vec3(0, 1, 0);
        green.intensity = 0.8f;

        auto& blue = Entity::create<Entity>().emplace<LightComponent>();
        blue.position = vec3(0, 1, 2);
        blue.color = vec3(0, 0, 1);
        blue.intensity = 0.8f;

#if 0
        auto& helmet = Entity::create<Entity>();
        helmet.emplace<ModelComponent>("assets/DamagedHelmet/glTF-Binary/DamagedHelmet.glb");
        helmet.emplace<EditorComponent>("Helmet");

#if 0
        auto& camera = Entity::create<Entity>();
        camera.emplace<ModelComponent>("assets/AntiqueCamera/glTF-Binary/AntiqueCamera.glb");
        camera.emplace<EditorComponent>("Camera");

        auto& fox = Entity::create<Entity>();
        fox.emplace<ModelComponent>("assets/Fox/glTF-Binary/Fox.glb");
        fox.emplace<EditorComponent>("Fox");
#endif

        auto& duck = Entity::create<Entity>();
        duck.emplace<ModelComponent>("assets/Duck/glTF-Binary/Duck.glb");
        duck.emplace<EditorComponent>("Duck");
        duck.get<TransformComponent>() = mat4(0.01f);
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
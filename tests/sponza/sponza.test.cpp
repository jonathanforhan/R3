#include <R3>
#include <R3_components>
#include <R3_core>
#include <R3_input>

using namespace R3;

class Helmet : public Entity {
public:
    void init() { emplace<ModelComponent>("assets/DamagedHelmet/glTF-Binary/DamagedHelmet.glb"); }
};

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
        auto& camera = cameraEntity.emplace<CameraComponent>();
        camera.setActive(true);
        camera.setPosition(vec3(0, 0, -3));

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

#if 1
        auto& helmet = Entity::create<Helmet>();
        helmet.emplace<EditorComponent>("Helmet");

        Scene::bindEventListener([&](const KeyPressEvent& e) {
            if (e.payload.key == Key::Enter && helmet.valid()) {
                helmet.destroy();
            } else if (e.payload.key == Key::J) {
                Entity::forEach([](const EntityView& e) { LOG(Info, e.id()); });
            }
        });
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
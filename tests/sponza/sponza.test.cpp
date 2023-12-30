#include <R3>
#include <R3_components>
#include <R3_core>
#include <R3_input>

using namespace R3;

bool rotate = true;

class Helmet : public Entity {
public:
    void init() { emplace<ModelComponent>("assets/DamagedHelmet/glTF-Binary/DamagedHelmet.glb"); }

    void tick(double dt) {
        if (rotate) {
            auto& transform = get<TransformComponent>();
            transform = glm::rotate(transform, (float)dt, vec3(0.0f, 1.0f, 0.0f));
        }
    }
};

extern "C" {

R3_DLL void* Entry() {
    static Scene scene = Scene(HASH32("foobar"));
    CurrentScene = &scene;
    return CurrentScene;
}

R3_DLL void Run() {
    try {
        //--- Camera
        auto& cameraEntity = Entity::create<Entity>();
        auto& camera = cameraEntity.emplace<CameraComponent>();
        camera.setActive(true);
        camera.setPosition(vec3(0, 0, 0));

//--- Model
#if 0
        auto& helmet = Entity::create<Helmet>();

        Scene::bindEventListener([&](const KeyPressEvent& e) {
            if (e.payload.key == Key::Space && helmet.valid())
                rotate = !rotate;
            if (e.payload.key == Key::Enter && helmet.valid())
                helmet.destroy();
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
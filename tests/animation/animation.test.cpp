#include <R3>
#include <R3_components>
#include <R3_core>
#include <R3_input>

using namespace R3;

extern "C" {

R3_DLL void* Entry() {
    CurrentScene = new Scene(HASH32("Animation"), "Animation");
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
        auto& cam = Entity::create<Entity>().emplace<CameraComponent>();
        cam.setActive(true);
        cam.translateBackward(10);

        auto& entity = Entity::create<Entity>();
        // entity.emplace<ModelComponent>("assets/glTF/Models/InterpolationTest/glTF-Binary/InterpolationTest.glb");
        // entity.emplace<ModelComponent>("assets/glTF/Models/AnimatedCube/glTF/AnimatedCube.gltf");
        // entity.emplace<ModelComponent>("assets/WalkingRobot/glTF/WalkingRobot.gltf");
        entity.emplace<ModelComponent>("assets/glTF/Models/ABeautifulGame/glTF/ABeautifulGame.gltf");
        entity.emplace<EditorComponent>().name = "Animation";
    } catch (std::exception const& e) {
        LOG(Error, e.what());
    }
}

} // extern "C"

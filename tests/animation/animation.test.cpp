#include <R3>
#include <R3_core>
#include <R3_input>
#include "components/CameraComponent.hpp"
#include "components/EditorComponent.hpp"
#include "components/LightComponent.hpp"
#include "components/ModelComponent.hpp"

// Models
// "assets/glTF/Models/AnimatedCube/glTF/AnimatedCube.gltf"
// "assets/glTF/Models/RiggedFigure/glTF-Binary/RiggedFigure.glb"
// "assets/glTF/Models/RiggedSimple/glTF-Binary/RiggedSimple.glb"
// "assets/glTF/Models/AnimatedMorphCube/glTF-Binary/AnimatedMorphCube.glb"
// "assets/glTF/Models/ABeautifulGame/glTF/ABeautifulGame.gltf"
// "assets/glTF/Models/SimpleSkin/glTF/SimpleSkin.gltf"
// "assets/WalkingRobot/glTF/WalkingRobot.gltf"

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
        cam.setPosition(vec3(0, 10, 0));

        auto& light1 = Entity::create<Entity>();
        auto& lc = light1.emplace<LightComponent>();
        lc.intensity = 3.0f;
        lc.position = vec3(0, 10, 0);
        light1.emplace<EditorComponent>().name = "Light";

        auto& robot = Entity::create<Entity>();
        robot.emplace<ModelComponent>("assets/WalkingRobot/glTF/WalkingRobot.gltf").skeleton.animated = true;
        robot.emplace<EditorComponent>().name = "Robot";

        auto& phoenix = Entity::create<Entity>();
        phoenix.emplace<ModelComponent>("assets/Phoenix/glTF-Binary/Phoenix.glb").skeleton.animated = true;
        phoenix.emplace<EditorComponent>().name = "Phoenix";

    } catch (std::exception const& e) {
        LOG(Error, e.what());
    }
}

} // extern "C"

#include <R3>
#include <R3_components>
#include <R3_core>
#include <R3_input>

// Models
// "assets/glTF/Models/AnimatedCube/glTF/AnimatedCube.gltf"
// "assets/glTF/Models/RiggedFigure/glTF-Binary/RiggedFigure.glb"
// "assets/glTF/Models/AnimatedMorphCube/glTF-Binary/AnimatedMorphCube.glb"
// "assets/glTF/Models/ABeautifulGame/glTF/ABeautifulGame.gltf"
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

        auto& cube = Entity::create<Entity>();
        cube.emplace<ModelComponent>("assets/glTF/Models/AnimatedCube/glTF/AnimatedCube.gltf");
        cube.emplace<EditorComponent>().name = "Cube";
        cube.get<TransformComponent>() = glm::translate(mat4(1.0f), vec3(0, 0, -20));

        auto& robot = Entity::create<Entity>();
        robot.emplace<ModelComponent>("assets/WalkingRobot/glTF/WalkingRobot.gltf");
        robot.emplace<EditorComponent>().name = "Robot";
        auto& t = robot.get<TransformComponent>();
        t = mat4(0.1f);
        t = glm::translate(t, vec3(0, 0, -4));
    } catch (std::exception const& e) {
        LOG(Error, e.what());
    }
}

} // extern "C"

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

R3_DLL void* Entry(void) {
    CurrentScene = new Scene(HASH32("Animation"), "Animation");
    return CurrentScene;
}

R3_DLL void Exit(void* scene_) {
    auto* scene = static_cast<Scene*>(scene_);
    scene->clearRegistry();
    delete scene;
}

R3_DLL void Run(void) {
    try {
        //--- Camera
        auto& cam = Entity::create<Entity>().emplace<CameraComponent>();
        cam.setActive(true);
        cam.translateBackward(10);
        cam.setPosition(vec3(10, 2, 0));

        auto& light1 = Entity::create<Entity>();
        auto& lc = light1.emplace<LightComponent>();
        lc.intensity = 100.0f;
        lc.position = vec3(0, 4, 0);
        light1.emplace<EditorComponent>().name = "Light";

        {
            auto& robot = Entity::create<Entity>();
            robot.emplace<ModelComponent>("assets/WalkingRobot/glTF/WalkingRobot.gltf").animation.running = true;
            robot.emplace<EditorComponent>().name = "Robot";
            auto& t = robot.get<TransformComponent>();
            t = glm::translate(t, vec3(0, -0.8, 0));
            t = glm::scale(t, vec3(0.15f));
        }

        {
            auto& phoenix = Entity::create<Entity>();
            phoenix.emplace<ModelComponent>("assets/Phoenix/glTF-Binary/Phoenix.glb").animation.running = true;
            phoenix.emplace<EditorComponent>().name = "Phoenix";
            auto& t = phoenix.get<TransformComponent>();
            t = glm::translate(t, vec3(0, 1, 0));
            t = glm::scale(t, vec3(0.01f));
        }

        {
            auto& sponza = Entity::create<Entity>();
            sponza.emplace<ModelComponent>("assets/glTF/Models/Sponza/glTF/Sponza.gltf");
            sponza.emplace<EditorComponent>().name = "Sponza";
            auto& t = sponza.get<TransformComponent>();
            t = glm::translate(t, vec3(0, -3, 0));
            t = glm::scale(t, vec3(0.02f));
        }

    } catch (std::exception const& e) {
        LOG(Error, e.what());
    }
}

} // extern "C"

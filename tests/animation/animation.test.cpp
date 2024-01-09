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

struct Armature : public Entity {
public:
    void init() {
        auto& cam = emplace<CameraComponent>();
        cam.active = true;

        auto& m = emplace<ModelComponent>("assets/person.glb");
        m.currentAnimation = "idle";
        emplace<EditorComponent>("Armature");
        auto& t = get<TransformComponent>();
        t = glm::rotate(t, glm::radians(90.0f), vec3(1, 0, 0));
        t = glm::scale(t, vec3(0.01f));

        Scene::bindEventListener([this](const KeyPressEvent& e) {
            switch (e.payload.key) {
                case Key::W:
                    activeKeys.w = true;
                    break;
                case Key::A:
                    activeKeys.a = true;
                    break;
                case Key::S:
                    activeKeys.s = true;
                    break;
                case Key::D:
                    activeKeys.d = true;
                    break;
                case Key::E:
                    activeKeys.e = true;
                    break;
                case Key::Q:
                    activeKeys.q = true;
                    break;
                default:
                    break;
            }
        });

        Scene::bindEventListener([this](const KeyReleaseEvent& e) {
            auto& m = get<ModelComponent>();

            switch (e.payload.key) {
                case Key::W:
                    m.currentAnimation = "idle";
                    m.animations["idle"].currentTime = 0.0f;
                    m.animations["walking"].currentTime = 0.0f;
                    m.animations["left_strafe"].currentTime = 0.0f;
                    m.animations["right_strafe"].currentTime = 0.0f;
                    activeKeys.w = false;
                    break;
                case Key::A:
                    activeKeys.a = false;
                    break;
                case Key::S:
                    activeKeys.s = false;
                    break;
                case Key::D:
                    activeKeys.d = false;
                    break;
                case Key::E:
                    activeKeys.e = false;
                    break;
                case Key::Q:
                    activeKeys.q = false;
                    break;
                default:
                    break;
            }
        });

        Scene::bindEventListener([this](const MouseButtonPressEvent& e) {
            if (e.payload.button == MouseButton::Left)
                mouseDown = true;
        });

        Scene::bindEventListener([this](const MouseButtonReleaseEvent& e) {
            if (e.payload.button == MouseButton::Left)
                mouseDown = false;
        });

        Scene::bindEventListener([this](const MouseCursorEvent& e) { cursorPosition = e.payload.cursorPosition; });
    }

    void tick(double dt) {
        float deltaT = static_cast<float>(dt);

        auto& m = get<ModelComponent>();
        auto& t = get<TransformComponent>();
        auto& pos = t[3];
        auto& cam = get<CameraComponent>();

        if (m.currentAnimation == "left_turn") {
            if (m.animations[*m.currentAnimation].currentTime == 0) {
                m.currentAnimation = "idle";
                t = glm::rotate(t, -glm::pi<float>() / 2, vec3(0, 0, 1));
            }
            cam.yaw -= ((90.0f / m.animations[*m.currentAnimation].maxTime) * deltaT);
        } else if (m.currentAnimation == "right_turn") {
            if (m.animations[*m.currentAnimation].currentTime == 0) {
                m.currentAnimation = "idle";
                t = glm::rotate(t, glm::pi<float>() / 2, vec3(0, 0, 1));
            }
            cam.yaw += ((90.0f / m.animations[*m.currentAnimation].maxTime) * deltaT);
        }

        static constexpr float mouseSensitivity = 360.0f;
        static constexpr float movementSensitivity = 8.0f;

        const float mag = deltaT * movementSensitivity;

        if (activeKeys.w) {
            if (activeKeys.a) {
                m.currentAnimation = "left_strafe";
                pos += vec4(cam.frontPlanar() * mag, 0);
            } else if (activeKeys.d) {
                m.currentAnimation = "right_strafe";
                pos += vec4(cam.frontPlanar() * mag, 0);
            } else if (m.currentAnimation != "left_turn" && m.currentAnimation != "right_turn") {
                m.currentAnimation = "walking";
                pos += vec4(cam.frontPlanar() * mag, 0);
            }
        } else if (activeKeys.s) {
            pos += vec4(cam.backPlanar() * mag, 0);
        }

        if (activeKeys.a) {
            pos += vec4(cam.left() * mag / 2.0f, 0);
        } else if (activeKeys.d) {
            pos += vec4(cam.right() * mag / 2.0f, 0);
        }

        if (activeKeys.q) {
            pos += vec4(cam.up * mag, 0);
        } else if (activeKeys.e) {
            pos -= vec4(cam.up * mag, 0);
        }

        const float deltaX = mouseDown ? cursorPosition.x - prevCursorPosition.x : 0.0f;
        const float deltaY = mouseDown ? -cursorPosition.y + prevCursorPosition.y : 0.0f;
        prevCursorPosition = cursorPosition;

        cam.pitch += deltaY * mouseSensitivity;

        float oldYaw = cam.yaw;
        cam.yaw += deltaX * mouseSensitivity;
        float deltaYaw = cam.yaw - oldYaw;

        if (deltaYaw > 4.0f) {
            m.currentAnimation = "right_turn";
        } else if (deltaYaw < -4.0f) {
            m.currentAnimation = "left_turn";
        }

        // keep player facing camera
        t = glm::rotate(t, glm::radians(deltaYaw), vec3(0, 0, 1));

        // move camera to player location
        cam.target = t[3];
    }

public:
    struct ActiveKeys {
        bool w = false;
        bool a = false;
        bool s = false;
        bool d = false;
        bool e = false;
        bool q = false;
    } activeKeys;
    bool mouseDown = false;

    vec2 cursorPosition = vec2(0.0f);
    vec2 prevCursorPosition = vec2(0.0f);
};

extern "C" {

R3_DLL void* Entry() {
    CurrentScene = new Scene(HASH32("Animation"), "Animation");
    return CurrentScene;
}

R3_DLL void Exit(void* scene_) {
    auto* scene = static_cast<Scene*>(scene_);
    scene->clearRegistry();
    delete scene;
}

R3_DLL void Run() {
    try {
        //--- Camera

        for (int i = 0; i < 16; i++) {
            auto& light = Entity::create<Entity>();
            light.emplace<EditorComponent>(std::format("Light {}", i));
            auto& lc = light.emplace<LightComponent>();
            lc.intensity = 16.0f;
            lc.position = vec3(-11.25, 6, i * -5);
        }

        Entity::create<Armature>();

        {
            auto& garage = Entity::create<Entity>();
            garage.emplace<ModelComponent>("assets/garage.glb");
            garage.emplace<EditorComponent>("Garage");
            auto& t = garage.get<TransformComponent>();
            t = glm::rotate(t, glm::radians(270.0f), vec3(1, 0, 0));
            t = glm::scale(t, vec3(0.025f));
        }

    } catch (std::exception const& e) {
        LOG(Error, e.what());
    }
}

} // extern "C"

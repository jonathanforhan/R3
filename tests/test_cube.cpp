#include "api/Check.hpp"
#include "api/Log.hpp"
#include "api/Math.hpp"

// BAD INCLUDES
#include "core/Engine.hpp"
#include "core/Entity.hpp"
#include "core/Shader.hpp"
#include "core/Mesh.hpp"
#include "core/Texture2D.hpp"
#include "components/CameraComponent.hpp"
#include "systems/InputSystem.hpp"

using namespace R3;

Vertex vertices[] = {
    // positions         colors              texture coords
    {{0.5f, 0.5f, 0.5f}, {1.0f, 0.0f, 0.0f}, {1.0f, 1.0f}},   // top right
    {{0.5f, -0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},  // bottom right
    {{-0.5f, -0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}}, // bottom left
    {{-0.5f, 0.5f, 0.5f}, {1.0f, 1.0f, 0.0f}, {0.0f, 1.0f}},  // top left

    {{0.5f, 0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {1.0f, 1.0f}},   // top right
    {{0.5f, -0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},  // bottom right
    {{-0.5f, -0.5f, -0.5f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}}, // bottom left
    {{-0.5f, 0.5f, -0.5f}, {1.0f, 1.0f, 0.0f}, {0.0f, 1.0f}},  // top left

    {{0.5f, 0.5f, 0.5f}, {1.0f, 0.0f, 0.0f}, {1.0f, 1.0f}},   // top right
    {{0.5f, 0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},  // bottom right
    {{0.5f, -0.5f, -0.5f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}}, // bottom left
    {{0.5f, -0.5f, 0.5f}, {1.0f, 1.0f, 0.0f}, {0.0f, 1.0f}},  // top left

    {{-0.5f, 0.5f, 0.5f}, {1.0f, 0.0f, 0.0f}, {1.0f, 1.0f}},   // top right
    {{-0.5f, 0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},  // bottom right
    {{-0.5f, -0.5f, -0.5f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}}, // bottom left
    {{-0.5f, -0.5f, 0.5f}, {1.0f, 1.0f, 0.0f}, {0.0f, 1.0f}},  // top left

    {{0.5f, 0.5f, 0.5f}, {1.0f, 0.0f, 0.0f}, {1.0f, 1.0f}},   // top right
    {{0.5f, 0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},  // bottom right
    {{-0.5f, 0.5f, -0.5f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}}, // bottom left
    {{-0.5f, 0.5f, 0.5f}, {1.0f, 1.0f, 0.0f}, {0.0f, 1.0f}},  // top left

    {{0.5f, -0.5f, 0.5f}, {1.0f, 0.0f, 0.0f}, {1.0f, 1.0f}},   // top right
    {{0.5f, -0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},  // bottom right
    {{-0.5f, -0.5f, -0.5f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}}, // bottom left
    {{-0.5f, -0.5f, 0.5f}, {1.0f, 1.0f, 0.0f}, {0.0f, 1.0f}},  // top left
};
uint32 indices[] = {
    0,  1,  3,  1,  2,  3,  4,  5,  7,  5,  6,  7,  8,  9,  11, 9,  10, 11,
    12, 13, 15, 13, 14, 15, 16, 17, 19, 17, 18, 19, 20, 21, 23, 21, 22, 23,
};

struct Cube : public Entity {
    Cube(const vec3& position)
        : transform(1.0f),
          shader(ShaderType::GLSL, "shaders/default.vert", "shaders/default.frag"),
          mesh(vertices, indices),
          texture("textures/container.jpg") {

        texture.bind(0);
        shader.writeUniform(shader.location("u_Texture"), 0);
        transform = glm::translate(transform, position);
    }

    void tick(double dt) override {
        transform = glm::rotate(transform, glm::radians((float)dt * 10.0f), vec3(0.0f, 1.0f, 0.0f));

        texture.bind(0);
        shader.bind();
        mesh.bind();

        shader.writeUniform(shader.location("u_Model"), transform);
        shader.writeUniform(shader.location("u_View"), Engine::activeScene().view);
        shader.writeUniform(shader.location("u_Projection"), Engine::activeScene().projection);

        Engine::renderer().drawElements(RenderPrimitive::Triangles, mesh.indexCount());
    }

    mat4 transform;
    Shader shader;
    Mesh mesh;
    Texture2D texture;
};

struct Player : public Entity {};

class CameraSystem : public InputSystem {
public:
    CameraSystem() {
        constexpr float s = 4.0f;
        setKeyBinding(Key::Key_W, [this](InputAction) {
            Engine::activeScene().componentView<CameraComponent>().each([this](CameraComponent& camera) {
                if (camera.active())
                    camera.translateForward(deltaTime * s);
            });
        });
        setKeyBinding(Key::Key_A, [this](InputAction) {
            Engine::activeScene().componentView<CameraComponent>().each([this](CameraComponent& camera) {
                if (camera.active())
                    camera.translateLeft(deltaTime * s);
            });
        });
        setKeyBinding(Key::Key_S, [this](InputAction) {
            Engine::activeScene().componentView<CameraComponent>().each([this](CameraComponent& camera) {
                if (camera.active())
                    camera.translateBackward(deltaTime * s);
            });
        });
        setKeyBinding(Key::Key_D, [this](InputAction) {
            Engine::activeScene().componentView<CameraComponent>().each([this](CameraComponent& camera) {
                if (camera.active())
                    camera.translateRight(deltaTime * s);
            });
        });
        setKeyBinding(Key::Key_Q, [this](InputAction) {
            Engine::activeScene().componentView<CameraComponent>().each([this](CameraComponent& camera) {
                if (camera.active())
                    camera.translateUp(deltaTime * s);
            });
        });
        setKeyBinding(Key::Key_E, [this](InputAction) {
            Engine::activeScene().componentView<CameraComponent>().each([this](CameraComponent& camera) {
                if (camera.active())
                    camera.translateDown(deltaTime * s);
            });
        });

        setMouseBinding(MouseButton::Left, [this](InputAction action) {
            if (action == InputAction::Press) {
                mouseDown = true;
            } else if (action == InputAction::Release) {
                mouseDown = false;
            }
        });
    }

    void tick(double dt) {
        deltaTime = static_cast<float>(dt);
        InputSystem::tick(dt);

        auto [xPos, yPos] = cursorPosition();
        double dx{}, dy{}, sensitivity = 500;
        if (mouseDown) {
            dx = xPos - prevCursorPosition.x;
            dy = -yPos + prevCursorPosition.y;
        }
        prevCursorPosition = dvec2(xPos, yPos);

        Engine::activeScene().componentView<CameraComponent>().each([this, dx, dy, sensitivity](CameraComponent& camera) {
            if (camera.active()) {
                Scene& scene = Engine::activeScene();
                if (mouseDown) {
                    camera.lookAround(static_cast<float>(dx * sensitivity), static_cast<float>(dy * sensitivity));
                }
                camera.apply(&scene.view, &scene.projection, Engine::window().aspectRatio());
            }
        });
    }

    float deltaTime = 0.0f;
    bool mouseDown = false;
    dvec2 prevCursorPosition = dvec2(0.0, 0.0);
};

int main(void) {
#if defined _DEBUG || !defined NDEBUG
    LOG(Info, "Running Debug Cube Test...");
#elif
    LOG(Info, "Running Release Cube Test...");
#endif

    Scene& defaultScene = Engine::addScene("default", true);
    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 5; j++) {
            for (int k = 0; k < 5; k++) {
                (void)Entity::create<Cube>(&defaultScene, vec3(i * 10, j * 10, k * 10));
            }
        }
    }

    Player& player = Entity::create<Player>(&defaultScene);
    CameraComponent& camera = player.emplace<CameraComponent>();
    camera.setActive();
    Engine::activeScene().addSystem<CameraSystem>();

    Engine::loop();
}

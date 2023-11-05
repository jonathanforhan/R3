#include "api/Check.hpp"
#include "api/Log.hpp"
#include "api/Math.hpp"
#include "core/Engine.hpp"
#include "core/Entity.hpp"

#include "components/CameraComponent.hpp"
#include "core/Mesh.hpp"
#include "core/Shader.hpp"
#include "core/Texture2D.hpp"
#include "systems/CameraSystem.hpp"

using namespace R3;

vec3 g_lightPosition = vec3(1.2f, 1.0f, 2.0f);
vec3 g_lightColor = vec3(1, 1, 1);

Vertex vertices[] = {
    {{-0.5f, -0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, {0.0f, 0.0f}},
    {{0.5f, -0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, {1.0f, 0.0f}},
    {{0.5f, 0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, {1.0f, 1.0f}},
    {{0.5f, 0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, {1.0f, 1.0f}},
    {{-0.5f, 0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, {0.0f, 1.0f}},
    {{-0.5f, -0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, {0.0f, 0.0f}},

    {{-0.5f, -0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}},
    {{0.5f, -0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f}},
    {{0.5f, 0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},
    {{0.5f, 0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},
    {{-0.5f, 0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}},
    {{-0.5f, -0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}},

    {{-0.5f, 0.5f, 0.5f}, {-1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
    {{-0.5f, 0.5f, -0.5f}, {-1.0f, 0.0f, 0.0f}, {1.0f, 1.0f}},
    {{-0.5f, -0.5f, -0.5f}, {-1.0f, 0.0f, 0.0f}, {0.0f, 1.0f}},
    {{-0.5f, -0.5f, -0.5f}, {-1.0f, 0.0f, 0.0f}, {0.0f, 1.0f}},
    {{-0.5f, -0.5f, 0.5f}, {-1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
    {{-0.5f, 0.5f, 0.5f}, {-1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},

    {{0.5f, 0.5f, 0.5f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
    {{0.5f, 0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {1.0f, 1.0f}},
    {{0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f}},
    {{0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f}},
    {{0.5f, -0.5f, 0.5f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
    {{0.5f, 0.5f, 0.5f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},

    {{-0.5f, -0.5f, -0.5f}, {0.0f, -1.0f, 0.0f}, {0.0f, 1.0f}},
    {{0.5f, -0.5f, -0.5f}, {0.0f, -1.0f, 0.0f}, {1.0f, 1.0f}},
    {{0.5f, -0.5f, 0.5f}, {0.0f, -1.0f, 0.0f}, {1.0f, 0.0f}},
    {{0.5f, -0.5f, 0.5f}, {0.0f, -1.0f, 0.0f}, {1.0f, 0.0f}},
    {{-0.5f, -0.5f, 0.5f}, {0.0f, -1.0f, 0.0f}, {0.0f, 0.0f}},
    {{-0.5f, -0.5f, -0.5f}, {0.0f, -1.0f, 0.0f}, {0.0f, 1.0f}},

    {{-0.5f, 0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f}},
    {{0.5f, 0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 1.0f}},
    {{0.5f, 0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
    {{0.5f, 0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
    {{-0.5f, 0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
    {{-0.5f, 0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f}},
};

uint32 indices[] = {
    0,  1,  2,  3,  4,  5,  6,  7,  8,  9,  10, 11, 12, 13, 14, 15, 16, 17,
    18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35,
};

struct Cube : Entity {
    Cube(const vec3& position, const vec3& scale)
        : shader(ShaderType::GLSL, "shaders/light_map.vert", "shaders/light_map.frag"),
          mesh(vertices, indices),
          texture("textures/crate.png"),
          textureSpecular("textures/crate_specular.png") {
        texture.bind(0);
        textureSpecular.bind(1);
        shader.bind();
        shader.writeUniform(shader.location("u_Material.diffuse"), 0);
        shader.writeUniform(shader.location("u_Material.specular"), 1);
        shader.writeUniform(shader.location("u_Material.shininess"), 64.0f);

        transform = glm::translate(mat4(1.0f), position);
        transform = glm::scale(transform, scale);
    }

    void tick(double) {
        shader.bind();
        mesh.bind();

        shader.writeUniform(shader.location("u_Model"), transform);
        shader.writeUniform(shader.location("u_View"), Engine::activeScene().view);
        shader.writeUniform(shader.location("u_Projection"), Engine::activeScene().projection);

        shader.writeUniform(shader.location("u_Light.position"), g_lightPosition);
        Engine::activeScene().componentView<CameraComponent>().each([&](CameraComponent& camera) {
            shader.writeUniform(shader.location("u_ViewPosition"), camera.position());
        });
        shader.writeUniform(shader.location("u_Light.ambient"), vec3(0.2f, 0.2f, 0.2f));
        shader.writeUniform(shader.location("u_Light.diffuse"), vec3(0.5f, 0.5f, 0.5f));
        shader.writeUniform(shader.location("u_Light.specular"), vec3(1.0f, 1.0f, 1.0f));

        texture.bind(0);
        textureSpecular.bind(1);

        Engine::renderer().drawElements(RenderPrimitive::Triangles, mesh.indexCount());
    }
    mat4 transform{1.0f};
    Shader shader;
    Mesh mesh;
    Texture2D texture;
    Texture2D textureSpecular;
};

struct Floor : Entity {
    Floor(const vec3& position, const vec3& scale)
        : shader(ShaderType::GLSL, "shaders/default.vert", "shaders/default.frag"),
          mesh(vertices, indices) {
        transform = glm::translate(mat4(1.0f), position);
        transform = glm::scale(transform, scale);
        shader.bind();
        shader.writeUniform(shader.location("u_Color"), vec3(1, 0.4, 1));
    }

    void tick(double) {
        shader.bind();
        mesh.bind();

        shader.writeUniform(shader.location("u_Model"), transform);
        shader.writeUniform(shader.location("u_View"), Engine::activeScene().view);
        shader.writeUniform(shader.location("u_Projection"), Engine::activeScene().projection);

        shader.writeUniform(shader.location("u_Light.position"), g_lightPosition);
        Engine::activeScene().componentView<CameraComponent>().each([&](CameraComponent& camera) {
            shader.writeUniform(shader.location("u_ViewPosition"), camera.position());
        });
        shader.writeUniform(shader.location("u_Light.ambient"), vec3(0.2f, 0.2f, 0.2f));
        shader.writeUniform(shader.location("u_Light.diffuse"), vec3(0.5f, 0.5f, 0.5f));
        shader.writeUniform(shader.location("u_Light.specular"), vec3(1.0f, 1.0f, 1.0f));

        Engine::renderer().drawElements(RenderPrimitive::Triangles, mesh.indexCount());
    }
    mat4 transform{1.0f};
    Shader shader;
    Mesh mesh;
};

struct LightCube : Entity {
    LightCube(const vec3& position)
        : transform(mat4(1.0f)),
          shader(ShaderType::GLSL, "shaders/light.vert", "shaders/light.frag"),
          mesh(vertices, indices) {}

    void tick(double dt) {
        shader.bind();
        mesh.bind();

        static double tt = 0;
        tt += dt;
        g_lightPosition = vec3(5 * cos(tt), 1, 5 * sin(tt));
        transform = glm::translate(mat4(1.0f), g_lightPosition);

        shader.writeUniform(shader.location("u_Model"), transform);
        shader.writeUniform(shader.location("u_View"), Engine::activeScene().view);
        shader.writeUniform(shader.location("u_Projection"), Engine::activeScene().projection);

        shader.writeUniform(shader.location("u_Color"), g_lightColor);

        Engine::renderer().drawElements(RenderPrimitive::Triangles, mesh.indexCount());
    }

    mat4 transform;
    Shader shader;
    Mesh mesh;
};

struct Player : Entity {};

int main(void) {
#if defined _DEBUG || !defined NDEBUG
    LOG(Info, "Running Debug Cube Test...");
#elif
    LOG(Info, "Running Release Cube Test...");
#endif

    Scene& defaultScene = Engine::addScene("default", true);
    Entity::create<Floor>(&defaultScene, vec3(0.0f), vec3(50, 0.1, 50));
    Entity::create<Cube>(&defaultScene, vec3(0, 0.55, 0), vec3(1));
    Entity::create<LightCube>(&defaultScene, g_lightPosition);

    Player& player = Entity::create<Player>(&defaultScene);
    player.emplace<CameraComponent>().setActive();
    Engine::activeScene().addSystem<CameraSystem>();

    Engine::loop();
}

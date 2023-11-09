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

Vertex vertices[] = {
    {{-0.5f, -0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, {0.0f, 0.0f}},
    {{0.5f, 0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, {1.0f, 1.0f}},
    {{0.5f, -0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, {1.0f, 0.0f}},
    {{0.5f, 0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, {1.0f, 1.0f}},
    {{-0.5f, -0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, {0.0f, 0.0f}},
    {{-0.5f, 0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, {0.0f, 1.0f}},

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
    {{0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f}},
    {{0.5f, 0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {1.0f, 1.0f}},
    {{0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f}},
    {{0.5f, 0.5f, 0.5f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
    {{0.5f, -0.5f, 0.5f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},

    {{-0.5f, -0.5f, -0.5f}, {0.0f, -1.0f, 0.0f}, {0.0f, 1.0f}},
    {{0.5f, -0.5f, -0.5f}, {0.0f, -1.0f, 0.0f}, {1.0f, 1.0f}},
    {{0.5f, -0.5f, 0.5f}, {0.0f, -1.0f, 0.0f}, {1.0f, 0.0f}},
    {{0.5f, -0.5f, 0.5f}, {0.0f, -1.0f, 0.0f}, {1.0f, 0.0f}},
    {{-0.5f, -0.5f, 0.5f}, {0.0f, -1.0f, 0.0f}, {0.0f, 0.0f}},
    {{-0.5f, -0.5f, -0.5f}, {0.0f, -1.0f, 0.0f}, {0.0f, 1.0f}},

    {{-0.5f, 0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f}},
    {{0.5f, 0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
    {{0.5f, 0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 1.0f}},
    {{0.5f, 0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
    {{-0.5f, 0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f}},
    {{-0.5f, 0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
};
uint32 indices[] = {
    0,  1,  2,  3,  4,  5,  6,  7,  8,  9,  10, 11, 12, 13, 14, 15, 16, 17,
    18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35,
};

struct LightCube : Entity {
    LightCube(const vec3& position, const vec3& color, Shader& shader, Mesh& mesh)
        : position(position),
          transform(1.0f),
          color(color),
          shader(shader),
          mesh(mesh) {

        transform = glm::translate(mat4(1.0f), position);
    }

    void tick(double dt) {
        shader.bind();
        mesh.bind();

        static double tt = 0;
        tt += dt;

        shader.writeUniform("u_Model", transform);
        shader.writeUniform("u_View", Engine::activeScene().view);
        shader.writeUniform("u_Projection", Engine::activeScene().projection);
        shader.writeUniform("u_Color", color);

        Engine::renderer().drawArrays(RenderPrimitive::Triangles, mesh.vertexCount());
    }

    vec3 position;
    mat4 transform;
    vec3 color;
    Shader& shader;
    Mesh& mesh;
};

struct Cube : Entity {
    Cube(const vec3& position,
         const vec3& scale,
         Shader& shader,
         Mesh& mesh,
         Texture2D& texture,
         Texture2D& textureSpecular,
         float shine)
        : transform(1.0f),
          shader(shader),
          mesh(mesh),
          texture(texture),
          textureSpecular(textureSpecular) {

        texture.bind(0);
        textureSpecular.bind(1);
        shader.bind();
        shader.writeUniform("u_Material.diffuse", 0);
        shader.writeUniform("u_Material.specular", 1);
        shader.writeUniform("u_Material.shininess", shine);

        transform = glm::translate(mat4(1.0f), position);
        transform = glm::scale(transform, scale);
    }

    void tick(double) {
        texture.bind(0);
        textureSpecular.bind(1);
        shader.bind();
        mesh.bind();

        // Vertex Shader
        shader.writeUniform("u_Model", transform);
        shader.writeUniform("u_View", Engine::activeScene().view);
        shader.writeUniform("u_Projection", Engine::activeScene().projection);

        // Fragment Shader
        usize i = 0;
        Engine::activeScene().componentView<LightCube>().each([this, &i](LightCube& lightCube) {
            if (i > 4)
                return;

            std::stringstream ss;
            ss << "u_Lights[" << char('0' + i) << ']';
            std::string name = ss.str();
            shader.writeUniform(name + ".position", lightCube.position);
            shader.writeUniform(name + ".ambient", vec3(0.05f));
            shader.writeUniform(name + ".diffuse", vec3(0.8f));
            shader.writeUniform(name + ".specular", vec3(1.0f));
            shader.writeUniform(name + ".constant", 1.0f);
            shader.writeUniform(name + ".linear", 0.09f);
            shader.writeUniform(name + ".quadratic", 0.032f);
            shader.writeUniform(name + ".color", lightCube.color);

            i++;
        });
        Engine::activeScene().componentView<CameraComponent>().each([this](CameraComponent& camera) {
            if (camera.active()) {
                shader.writeUniform("u_ViewPosition", camera.position());
            }
        });

        Engine::renderer().drawArrays(RenderPrimitive::Triangles, mesh.vertexCount());
    }
    mat4 transform;
    Shader& shader;
    Mesh& mesh;
    Texture2D& texture;
    Texture2D& textureSpecular;
};

struct Player : Entity {
    void init() {
        emplace<CameraComponent>().setActive();
        Engine::activeScene().addSystem<CameraSystem>();
    }
    void tick(double) {}
};

int main(void) {
    Scene& defaultScene = Engine::addScene("default", true);

    Shader lightShader(ShaderType::GLSL, "shaders/light.vert", "shaders/light.frag");
    Shader defaultShader(ShaderType::GLSL, "shaders/default.vert", "shaders/default.frag");
    Texture2D crateTexture("textures/crate.png");
    Texture2D crateTextureSpecular("textures/crate_specular.png");
    Texture2D containerTexture("textures/container.jpg");
    Mesh cube(vertices);

    // crates
    Entity::create<Cube>(&defaultScene, vec3(3, 0, 3), vec3(1), defaultShader, cube, crateTexture, crateTextureSpecular,
                         64.0f);
    Entity::create<Cube>(&defaultScene, vec3(3, 0, -3), vec3(1), defaultShader, cube, crateTexture,
                         crateTextureSpecular, 64.0f);
    Entity::create<Cube>(&defaultScene, vec3(-3, 0, 3), vec3(1), defaultShader, cube, crateTexture,
                         crateTextureSpecular, 64.0f);
    Entity::create<Cube>(&defaultScene, vec3(-3, 0, -3), vec3(1), defaultShader, cube, crateTexture,
                         crateTextureSpecular, 64.0f);

    // floor
    for (int i = -10; i < 10; i++)
        for (int j = -10; j < 10; j++)
            Entity::create<Cube>(&defaultScene, vec3(i, 0, j), vec3(1), defaultShader, cube, containerTexture, containerTexture, 1.0f);

    // lights
    Entity::create<LightCube>(&defaultScene, vec3(3, 4, 3), vec3(1), lightShader, cube);
    Entity::create<LightCube>(&defaultScene, vec3(3, 4, -3), vec3(1, 0, 0), lightShader, cube);
    Entity::create<LightCube>(&defaultScene, vec3(-3, 4, 3), vec3(0, 1, 0), lightShader, cube);
    Entity::create<LightCube>(&defaultScene, vec3(-3, 4, -3), vec3(0, 0, 1), lightShader, cube);

    Entity::create<Player>(&defaultScene);

    Engine::loop();

    return 0;
}

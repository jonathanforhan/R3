#include "api/Check.hpp"
#include "api/Log.hpp"
#include "api/Math.hpp"
#include "core/Engine.hpp"
#include "core/Entity.hpp"
#include "core/Mesh.hpp"
#include "core/Shader.hpp"
#include "core/Texture2D.hpp"

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


struct Transform {
    Transform(float i)
        : model(i) {}
    mat4 model;
};

class MyEnt : public Entity {
public:
    MyEnt()
        : _mesh(Mesh(vertices, indices)),
          _shader(ShaderType::GLSL, "shaders/default.vert", "shaders/default.frag"),
          _texture("textures/container.jpg", "u_texture") {
        _texture.bind(0);
        _shader.use();
        _shader.write_uniform(_shader.location(_texture.name()), 0);

        LOG(Info, _mesh.id(), _shader.id(), _texture.id());
    }

    ~MyEnt() {
        // _mesh.destroy();
        // _shader.destroy();
        // _texture.destroy();
    }

    void tick(double dt) override {
        Engine::instance().view = glm::rotate(Engine::instance().view, glm::radians(1.0f), vec3(0.0f, 1.0f, 0.0f));

        _texture.bind(0);
        _shader.use();

        _shader.write_uniform(0, mat4(1.0f));
        _shader.write_uniform(1, Engine::instance().view);
        _shader.write_uniform(2, Engine::instance().projection);

        _mesh.bind();
        Engine::instance().draw_indexed(RendererPrimitive::Triangles, _mesh.number_of_indices());
    }

private:
    Mesh _mesh;
    Shader _shader;
    Texture2D _texture;
};

int main(void) {
#if defined _DEBUG || !defined NDEBUG
    LOG(Info, "Running Debug Cube Test...");
#elif
    LOG(Info, "Running Release Cube Test...");
#endif

    Engine::initialize();

    std::cout << std::boolalpha;

    MyEnt& e0 = Entity::create<MyEnt>();
    MyEnt& e1 = Entity::create<MyEnt>();

    e0.emplace<Transform>(1.0f);
    e1.emplace<Transform>(1.0f);

    Engine::instance().add_system(
        [](double dt) { Scene::registery().view<MyEnt>().each([dt](MyEnt& entity) { entity.tick(dt); }); });

    #if 0
    Engine::instance().add_system([](double dt) {
        Scene::registery().view<Transform>().each(
            [dt](Transform& transform) { transform.model });
    });
    #endif

    Engine::instance().loop();
}

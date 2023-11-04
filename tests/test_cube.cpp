#include "api/Check.hpp"
#include "api/Log.hpp"
#include "api/Math.hpp"

// BAD INCLUDES
#include "core/Engine.hpp"
#include "core/Entity.hpp"
#include "core/Shader.hpp"
#include "core/Mesh.hpp"
#include <vector>

using namespace R3;

#if 0
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
#endif

float verts[] = {
    -0.5, -0.5, 0, //
    0.5,  -0.5, 0, //
    0,    0.5,  0, //
};
unsigned indices[] = {0, 1, 2};

struct MyEnt : public Entity {
    MyEnt()
        : shader(ShaderType::GLSL, "shaders/default.vert", "shaders/default.frag"),
          mesh(verts, indices) {}

    void tick(double dt) override {
        shader.bind();
        mesh.bind();
        Engine::renderer().drawElements(RenderPrimitive::Triangles, mesh.indexCount());
    }

    Shader shader;
    Mesh mesh;
};

struct MyEnt2 : public Entity {
    MyEnt2()
        : shader(ShaderType::GLSL, "shaders/default.vert", "shaders/default.frag"),
          mesh(verts, indices) {}

    void tick(double dt) override {
        LOG(Info, "MY ENT 2");
        shader.bind();
        mesh.bind();
        Engine::renderer().drawElements(RenderPrimitive::Triangles, mesh.indexCount());
    }

    Shader shader;
    Mesh mesh;
};

int main(void) {
#if defined _DEBUG || !defined NDEBUG
    LOG(Info, "Running Debug Cube Test...");
#elif
    LOG(Info, "Running Release Cube Test...");
#endif

    Scene& defaultScene = Engine::addScene("default", true);
    (void)Entity::create<MyEnt>(&defaultScene);
    (void)Entity::create<MyEnt2>(&defaultScene);

    Engine::loop();
}

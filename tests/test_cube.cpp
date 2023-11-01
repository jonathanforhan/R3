#include "api/Check.hpp"
#include "api/Log.hpp"
#include "api/Math.hpp"
#include "api/Clock.hpp"
#include "core/Actor.hpp"
#include "core/Engine.hpp"
#include "components/Draw.hpp"

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

class Cube : public Actor {
public:
  Cube(vec3 starting_pos) {
    set_mesh("M_Cube");
    set_shader("S_Default");
    set_texture("T_Grass");
    transform = glm::translate(transform, starting_pos);
  }

  void translate(vec3 pos) {
    transform = glm::translate(transform, pos);
  }

  void tick(double delta_time) override {
    transform = glm::rotate(transform, static_cast<float>(delta_time), vec3(0, 1, 0));
  }
};

int main(void) {
#if defined _DEBUG || !defined NDEBUG
  LOG(Info, "Running Debug Cube Test...");
#elif
  LOG(Info, "Running Release Cube Test...");
#endif

  Engine* engine = Engine::instance();

  engine->register_component("EC_Draw", new ec::Draw);
  engine->register_mesh("M_Cube", {vertices, indices});
  engine->register_shader("S_Default", {ShaderType::GLSL, "shaders/default.vert", "shaders/default.frag"});
  engine->register_texture2D("T_Grass", {"textures/grass.jpg", "u_texture"});

  for (int32 i = -20; i < 20; i++) {
    for (int32 j = -20; j < 20; j++) {
      for (int32 k = -20; k < 20; k++) {
        Cube* cube = new Cube({i, j, k});
        cube->add_component(engine->component_ptr("EC_Draw"));
        engine->add_entity(cube);
      }
    }
  }
  engine->loop();
}

#include "api/Log.hpp"
#include "api/Math.hpp"
#include "api/Clock.hpp"
#include "core/Actor.hpp"
#include "core/Engine.hpp"

using namespace R3;

class Cube : public Actor {
public:
  Cube() {
    float vertices[] = {
        // positions         colors            texture coords
        0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.0f, 1.0f, 1.0f, // top right
        0.5f,  -0.5f, 0.5f,  0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // bottom right
        -0.5f, -0.5f, 0.5f,  0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom left
        -0.5f, 0.5f,  0.5f,  1.0f, 1.0f, 0.0f, 0.0f, 1.0f, // top left

        0.5f,  0.5f,  -0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, // top right
        0.5f,  -0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // bottom right
        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom left
        -0.5f, 0.5f,  -0.5f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, // top left

        0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.0f, 1.0f, 1.0f, // top right
        0.5f,  0.5f,  -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // bottom right
        0.5f,  -0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom left
        0.5f,  -0.5f, 0.5f,  1.0f, 1.0f, 0.0f, 0.0f, 1.0f, // top left

        -0.5f, 0.5f,  0.5f,  1.0f, 0.0f, 0.0f, 1.0f, 1.0f, // top right
        -0.5f, 0.5f,  -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // bottom right
        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom left
        -0.5f, -0.5f, 0.5f,  1.0f, 1.0f, 0.0f, 0.0f, 1.0f, // top left

        0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.0f, 1.0f, 1.0f, // top right
        0.5f,  0.5f,  -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // bottom right
        -0.5f, 0.5f,  -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom left
        -0.5f, 0.5f,  0.5f,  1.0f, 1.0f, 0.0f, 0.0f, 1.0f, // top left

        0.5f,  -0.5f, 0.5f,  1.0f, 0.0f, 0.0f, 1.0f, 1.0f, // top right
        0.5f,  -0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // bottom right
        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom left
        -0.5f, -0.5f, 0.5f,  1.0f, 1.0f, 0.0f, 0.0f, 1.0f, // top left
    };
    uint32_t stride[] = {3, 3, 2};
    uint32_t indices[] = {
        0,  1,  3,  1,  2,  3,  4,  5,  7,  5,  6,  7,  8,  9,  11, 9,  10, 11,
        12, 13, 15, 13, 14, 15, 16, 17, 19, 17, 18, 19, 20, 21, 23, 21, 22, 23,
    };
    set_mesh({vertices, stride, indices});
    Shader shader;
    shader.import_source("shaders/default.vert", "shaders/default.frag");
    set_shader(shader);
    set_texture({"textures/container.jpg", "uni_texture"});

    model = glm::translate(model, vec3(0.5f, -0.5f, 0.0f));
  }

  void tick(double delta_time) override {
    model = glm::rotate(model, static_cast<float>(delta_time), vec3(0.0f, 0.0f, 1.0f));
  }
};

int main(void) {
  LOG(Info, "Running Cube Test...");
  Engine::initialize();

  Cube cube;

  Engine* engine = Engine::instance();
  engine->add_actor(&cube);

  while (engine->running()) {
    engine->update();
  }
}

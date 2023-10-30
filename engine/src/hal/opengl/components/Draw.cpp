#if R3_OPENGL

#include "components/Draw.hpp"
#include "api/Math.hpp"
#include "core/Actor.hpp"
#include "core/Engine.hpp"

namespace R3::ec {

void Draw::initialize() {
  Actor* actor = reinterpret_cast<Actor*>(parent);
  uint32 shader = actor->shader_id();
  uint32 texture = actor->texture_id();

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, texture);
  glUseProgram(shader);
  glUniform1i(glGetAttribLocation(shader, "u_texture"), 0);
}

void Draw::tick(double) {
  Actor* actor = reinterpret_cast<Actor*>(parent);
  Engine* engine = Engine::instance();

  uint32 mesh = actor->mesh_id();
  uint32 shader = actor->shader_id();
  uint32 texture = actor->texture_id();

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, texture);
  glUseProgram(shader);
  glUniformMatrix4fv(0, 1, GL_FALSE, glm::value_ptr(parent->transform));
  glUniformMatrix4fv(1, 1, GL_FALSE, glm::value_ptr(engine->view));
  glUniformMatrix4fv(2, 1, GL_FALSE, glm::value_ptr(engine->projection));
  glBindVertexArray(mesh);

  engine->draw_indexed(RendererPrimitive::Triangles, 36);
}

} // namespace R3::ec

#endif // OPENGL

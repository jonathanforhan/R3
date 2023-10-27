#include "Renderer.hpp"
// clang-format off
#include <glad/glad.h>
#include <GLFW/glfw3.h>
// clang-format on
#include <glm/gtc/matrix_transform.hpp>
#include "core/Window.hpp"

namespace R3::opengl {

Renderer::Renderer(Window* window)
    : _window(window),
      _shader(Shader::from_source("shaders/default.vert", "shaders/default.frag")),
      _texture("textures/container.jpg") {
  glEnable(GL_DEPTH_TEST);

  float vertices[] = {
      // positions        colors            texture coords
      0.5f,  0.5f,  0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, // top right
      0.5f,  -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // bottom right
      -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom left
      -0.5f, 0.5f,  0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f  // top left
  };
  uint32_t indices[] = {
      0, 1, 3, // first Triangle
      1, 2, 3  // second Triangle
  };
  glGenVertexArrays(1, &_vao);
  glGenBuffers(1, &_vbo);
  glGenBuffers(1, &_ebo);
  glBindVertexArray(_vao);

  glBindBuffer(GL_ARRAY_BUFFER, _vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), reinterpret_cast<void*>(0));
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), reinterpret_cast<void*>(3 * sizeof(float)));
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), reinterpret_cast<void*>(6 * sizeof(float)));
  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(1);
  glEnableVertexAttribArray(2);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);

  _texture.bind(0);
  _shader.use();

  _shader.write_uniform(_shader.location("uni_texture"), 0);
}

Renderer::~Renderer() {
  glDeleteVertexArrays(1, &_vao);
  glDeleteBuffers(1, &_vbo);
  glDeleteBuffers(1, &_ebo);
}

void Renderer::draw() {
  glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glm::mat4 model = glm::mat4(1.0f);
  model = glm::translate(model, glm::vec3(0.5f, -0.5f, 0.0f));
  model = glm::rotate(model, static_cast<float>(glfwGetTime()), glm::vec3(0.0f, 0.0f, 1.0f));

  glm::mat4 view = glm::mat4(1.0f);
  view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));

  glm::mat4 projection = glm::perspective(glm::radians(45.0f), _window->aspect_ratio(), 0.1f, 100.0f);

  _texture.bind(0);
  _shader.use();

  _shader.write_uniform(_shader.location("model"), model);
  _shader.write_uniform(_shader.location("view"), view);
  _shader.write_uniform(_shader.location("projection"), projection);

  glBindVertexArray(_vao);
  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

} // namespace R3::opengl

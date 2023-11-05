#pragma once
#include "api/Log.hpp"
#include "api/Math.hpp"
#include "core/Engine.hpp"
#include "core/Entity.hpp"

#include "components/CameraComponent.hpp"
#include "core/Mesh.hpp"
#include "core/Shader.hpp"
#include "core/Texture2D.hpp"

using namespace R3;

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

        transform = glm::translate(mat4(1.0f), position);
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


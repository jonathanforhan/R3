#pragma once
#include "api/Log.hpp"
#include "api/Math.hpp"
#include "core/Engine.hpp"
#include "core/Entity.hpp"

#include "components/CameraComponent.hpp"
#include "core/Mesh.hpp"
#include "core/Shader.hpp"
#include "core/Texture2D.hpp"
#include "systems/TickSystem.hpp"
#include "LightCube.hpp"

using namespace R3;

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

    void tick(double);

    mat4 transform;
    Shader& shader;
    Mesh& mesh;
    Texture2D& texture;
    Texture2D& textureSpecular;
};

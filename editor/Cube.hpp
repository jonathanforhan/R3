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
        : shine(shine),
          scale(scale),
          transform(1.0f),
          shader(shader),
          mesh(mesh),
          texture(texture),
          textureSpecular(textureSpecular) {

        transform = glm::translate(mat4(1.0f), position);
        transform = glm::scale(transform, scale);
    }

    void tick(double);

    float shine;
    vec3 scale;
    mat4 transform;
    Shader& shader;
    Mesh& mesh;
    Texture2D& texture;
    Texture2D& textureSpecular;
};

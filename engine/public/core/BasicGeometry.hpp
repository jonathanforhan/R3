#pragma once

#include "render/Vertex.hpp"

namespace R3 {

/// @brief Plane Geometry
inline void Plane(Vertex vertices[4], uint32 indices[6]) {
    vertices[0].position = {-1.0f, -1.0f, 1.0f};
    vertices[0].textureCoords = {0.0f, 0.0f};
    vertices[1].position = {-1.0f, 1.0f, 1.0f};
    vertices[1].textureCoords = {0.0f, 1.0f};
    vertices[2].position = {1.0f, -1.0f, 1.0f};
    vertices[2].textureCoords = {1.0f, 0.0f};
    vertices[3].position = {1.0f, 1.0f, 1.0f};
    vertices[3].textureCoords = {1.0f, 1.0f};
    indices[0] = 1;
    indices[1] = 0;
    indices[2] = 2;
    indices[3] = 2;
    indices[4] = 3;
    indices[5] = 1;
};

/// @brief Cube Geometry
constexpr void Cube(Vertex vertices[36]) {
    constexpr usize N_VERTS = 36;
    constexpr usize N_FVERTS = N_VERTS * 3;

    constexpr float fvertices[N_FVERTS] = {
        //--- positions ---//
        // clang-format off
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f, 
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f, 
        -1.0f, -1.0f,  1.0f,

         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        // clang-format on
    };

    for (uint32 i = 0; i < N_VERTS; i++) {
        vertices[i].position.x = fvertices[i * 3 + 0];
        vertices[i].position.y = fvertices[i * 3 + 1];
        vertices[i].position.z = fvertices[i * 3 + 2];
    }
};

/// @brief IndexedCube Geometry
constexpr void IndexedCube(Vertex vertices[8], uint16 indices[36]) {
    constexpr usize N_VERTS = 8;
    constexpr usize N_FVERTS = N_VERTS * 3;

    constexpr float fvertices[N_FVERTS] = {
        //--- positions ---//
        -1.0f, -1.0f, -1.0f, // [0]
        -1.0f, 1.0f,  -1.0f, // [1]
        1.0f,  -1.0f, -1.0f, // [2]
        1.0f,  1.0f,  -1.0f, // [3]

        -1.0f, -1.0f, 1.0f, // [4]
        -1.0f, 1.0f,  1.0f, // [5]
        1.0f,  -1.0f, 1.0f, // [6]
        1.0f,  1.0f,  1.0f, // [7]
    };

    for (uint32 i = 0; i < N_VERTS; i++) {
        vertices[i].position.x = fvertices[i * 3 + 0];
        vertices[i].position.y = fvertices[i * 3 + 1];
        vertices[i].position.z = fvertices[i * 3 + 2];
    }

    constexpr usize N_IDX = 36;
    constexpr uint16 uindices[N_IDX] = {
        1, 0, 2, 2, 3, 1,

        4, 0, 1, 1, 5, 4,

        2, 6, 7, 7, 3, 2,

        4, 5, 7, 7, 6, 4,

        1, 3, 7, 7, 5, 1,

        0, 4, 2, 2, 4, 6,
    };

    for (uint32 i = 0; i < N_IDX; i++) {
        indices[i] = uindices[i];
    }
};

} // namespace R3
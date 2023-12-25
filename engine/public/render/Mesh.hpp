#pragma once

/// @file Mesh.hpp

#include "render/GraphicsPipeline.hpp"
#include "render/IndexBuffer.hpp"
#include "render/Material.hpp"
#include "render/VertexBuffer.hpp"

namespace R3 {

/// @brief Mesh is a POD struct that contains Buffer IDs,
struct Mesh {
    DEFAULT_CONSTRUCT(Mesh);
    NO_COPY(Mesh);
    DEFAULT_MOVE(Mesh);

    /// @brief Destroy allocated buffers manually
    /// Used by ModelComponent
    void destroy();

    VertexBuffer::ID vertexBuffer;       ///< Vertex handle
    IndexBuffer<uint32>::ID indexBuffer; ///< Index handle
    GraphicsPipeline::ID pipeline;       ///< Pipeline handle for binding
    Material material;                   ///< Material
};

} // namespace R3

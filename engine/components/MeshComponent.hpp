#pragma once

#include "api/Types.hpp"

namespace R3 {

struct MeshComponent {
    usize vertexBufferIndex;
    usize vertexCount;
    usize indexBufferIndex;
    usize indexCount;
};

} // namespace R3
#pragma once

#include "api/Types.hpp"
#include "core/Entity.hpp"
#include "core/ResourceManager.hpp"

namespace R3 {

struct MeshComponent {
    Handle<vulkan::Buffer> vertexBufferIndex;
    usize vertexCount;
    Handle<vulkan::Buffer> indexBufferIndex;
    usize indexCount;
};

} // namespace R3
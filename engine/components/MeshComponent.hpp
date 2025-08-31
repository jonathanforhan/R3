#pragma once

#include "engine/api/Api.hpp"
#include "engine/api/Types.hpp"
#include "engine/core/Entity.hpp"
#include "engine/core/ResourceManager.hpp"

namespace R3 {

struct R3_API MeshComponent {
    Handle<vulkan::Buffer> vertexBufferIndex;
    usize vertexCount;
    Handle<vulkan::Buffer> indexBufferIndex;
    usize indexCount;
};

} // namespace R3
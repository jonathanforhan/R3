#pragma once

#include <array>
#include "render/DescriptorPool.hpp"
#include "render/RenderSpecification.hpp"
#include "render/TextureBuffer.hpp"
#include "render/UniformBuffer.hpp"

namespace R3 {

// cleaned up by mesh for ease of use
struct Material {
    void destroy();

    DescriptorPool::ID descriptorPool;
    PBRTextureResource textures;
    std::array<UniformBuffer::ID, MAX_FRAMES_IN_FLIGHT> uniforms;
};

} // namespace R3
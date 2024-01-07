#pragma once

#include <R3>
#include "render/DescriptorPool.hpp"
#include "render/RenderApi.hpp"
#include "render/TextureBuffer.hpp"
#include "render/UniformBuffer.hpp"

namespace R3 {

class R3_API Scene;

struct R3_API Material {
    DescriptorPool descriptorPool;
    UniformBuffer uniforms[MAX_FRAMES_IN_FLIGHT * 2];
    TexturePBR textures;
    uint32 pbrFlags = 0;
};

} // namespace R3
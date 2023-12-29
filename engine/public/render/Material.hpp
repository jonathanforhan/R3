#pragma once

#include <R3>
#include "render/Texture.hpp"

namespace R3 {

struct R3_API Material {
    DEFAULT_CONSTRUCT(Material);
    NO_COPY(Material);
    DEFAULT_MOVE(Material);

    void destroy() {}

    id descriptorPool;
    PBRTextureResource textures;
    id uniforms[3 /*MAX_FRAMES_IN_FLIGHT*/ * 2];
    uint32 pbrFlags = 0;
};

} // namespace R3
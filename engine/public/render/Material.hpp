#pragma once

#include <R3>
#include "render/RenderApi.hpp"
#include "render/Texture.hpp"

namespace R3 {

class R3_API Scene;

struct R3_API Material {
    DEFAULT_CONSTRUCT(Material);
    NO_COPY(Material);
    DEFAULT_MOVE(Material);

    void destroy(Scene* parentScene);

    id descriptorPool = ~id(0);
    PBRTextureResource textures;
    id uniforms[MAX_FRAMES_IN_FLIGHT * 2];
    uint32 pbrFlags = 0;
};

} // namespace R3
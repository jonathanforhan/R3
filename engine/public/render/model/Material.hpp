#pragma once

#include <R3>
#include "render/RenderApi.hpp"
#include "render/Texture.hpp"

namespace R3 {

class R3_API Scene;

struct R3_API Material {
    void destroy(Scene* parentScene);

    id descriptorPool = undefined;
    id uniforms[MAX_FRAMES_IN_FLIGHT * 2] = {};
    PBRTextureResource textures;
    uint32 pbrFlags = 0;
};

} // namespace R3
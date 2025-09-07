#pragma once

#include "engine/api/Api.hpp"
#include "engine/core/ResourceManager.hpp"

namespace R3 {

struct R3_API TextureLifetimeComponent {
    std::vector<Handle<Texture>> textures;
};

} // namespace R3

#pragma once

#include "engine/api/Api.hpp"
#include "engine/core/ResourceManager.hpp"
#include "engine/render/vulkan/vulkan-Texture.hpp"

namespace R3 {

struct R3_API TextureLifetimeComponent {
    std::vector<Handle<vulkan::Texture>> textures;
};

} // namespace R3

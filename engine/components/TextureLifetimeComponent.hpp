#pragma once

#include "core/ResourceManager.hpp"
#include "render/vulkan/vulkan-Texture.hpp"

namespace R3 {

struct TextureLifetimeComponent {
    std::vector<Handle<vulkan::Texture>> textures;
};

} // namespace R3

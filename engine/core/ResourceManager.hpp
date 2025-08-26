#pragma once

// TODO make this renderer agnostic
#if !R3_VULKAN
#error "Vulkan is currently the only supported RenderContext"
#else
#include "render/vulkan/vulkan-Buffer.hpp"
#include "render/vulkan/vulkan-Image.hpp"
#include "render/vulkan/vulkan-Texture.hpp"
#endif

#include <memory>
#include <utility>
#include <entt/resource/cache.hpp>
#include <entt/resource/resource.hpp>
#include "api/Class.hpp"
#include "api/Hash.hpp"
#include "api/Types.hpp"
#include "render/Flags.hpp"
#include "render/vulkan/vulkan-CommandBuffer.hpp"

namespace R3 {

template <typename T>
using Handle = entt::resource<T>;

class ResourceManagerSingleton {
private:
    R3_CTOR_DEFAULT(ResourceManagerSingleton);
    R3_COPY_DELETE(ResourceManagerSingleton);
    R3_MOVE_DELETE(ResourceManagerSingleton);

public:
    template <typename... Args>
    Handle<vulkan::Buffer> loadBuffer(hash::uuid id, Args&&... args) {
        return m_bufferCache.load((entt::id_type)(uint64)id, std::forward<Args>(args)...).first->second;
    }

    template <typename... Args>
    Handle<vulkan::Image> loadImage(hash::uuid id, Args&&... args) {
        return m_imageCache.load((entt::id_type)(uint64)id, std::forward<Args>(args)...).first->second;
    }

    template <typename... Args>
    Handle<vulkan::Texture> loadTexture(hash::uuid id, Args&&... args) {
        return m_textureCache.load((entt::id_type)(uint64)id, std::forward<Args>(args)...).first->second;
    }

    void clear() {
        m_bufferCache.clear();
        m_imageCache.clear();
        m_textureCache.clear();
    }

private:
    entt::resource_cache<vulkan::Buffer> m_bufferCache;
    entt::resource_cache<vulkan::Image> m_imageCache;
    entt::resource_cache<vulkan::Texture> m_textureCache;

private:
    friend struct ResourceManager;
    friend class EngineSingleton; // <- allow Engine to bind the context
};

struct ResourceManager {
    ResourceManagerSingleton* operator->() noexcept {
        static ResourceManagerSingleton instance;
        return &instance;
    }
};

} // namespace R3
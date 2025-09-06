#pragma once

// TODO make this renderer agnostic
#if !R3_VULKAN
#error "Vulkan is currently the only supported RenderContext"
#else
#include "engine/render/vulkan/vulkan-Texture.hpp"
#endif

#include <map>
#include <memory>
#include <utility>
#include <vector>
#include <entt/core/fwd.hpp>
#include <entt/resource/cache.hpp>
#include <entt/resource/resource.hpp>
#include "Engine.hpp"
#include "engine/api/Api.hpp"
#include "engine/api/Class.hpp"
#include "engine/api/Hash.hpp"
#include "engine/api/Types.hpp"
#include "engine/render/Buffer.hpp"
#include "engine/render/Image.hpp"
#include "engine/render/vulkan/vulkan-Cubemap.hpp"

namespace R3 {

template <typename T>
using Handle = entt::resource<T>;

class R3_API ResourceManager {
private:
    R3_CTOR_DEFAULT(ResourceManager);
    R3_COPY_DELETE(ResourceManager);
    R3_MOVE_DELETE(ResourceManager);

public:
    template <typename... Args>
    std::pair<Handle<Buffer>, bool> loadBuffer(hash::uuid id, Args&&... args) {
        auto&& [it, b] = m_bufferCache.load((entt::id_type)(uint64)id, std::forward<Args>(args)...);
        return {it->second, b};
    }

    template <typename... Args>
    std::pair<Handle<Image>, bool> loadImage(hash::uuid id, Args&&... args) {
        auto&& [it, b] = m_imageCache.load((entt::id_type)(uint64)id, std::forward<Args>(args)...);
        return {it->second, b};
    }

    template <typename... Args>
    std::pair<Handle<vulkan::Texture>, bool> loadTexture(hash::uuid id, Args&&... args) {
        auto&& [it, b] = m_textureCache.load((entt::id_type)(uint64)id, std::forward<Args>(args)...);
        return {it->second, b};
    }

    uint32 bindTexture(hash::uuid id, const vulkan::Texture& texture);

    uint32 bindTexture(hash::uuid id, const vulkan::Cubemap& cubemap);

    void unbindTexture(uint32 slot);

    template <typename T, typename... Args>
    T* newFrameScopedObject(Args&&... args) {
        T* obj = new T(std::forward<Args>(args)...);
        GEngine()->EventHandler().bindEventListener("frame-done", [obj] noexcept {
            delete obj;
            return true; // remove listener after called once
        });
        return obj;
    }

    void clear();

private:
    entt::resource_cache<Buffer> m_bufferCache;
    entt::resource_cache<Image> m_imageCache;
    entt::resource_cache<vulkan::Texture> m_textureCache;

    std::map<uint64, uint32> m_textureBindMap;
    std::vector<std::pair<uint32, uint64>> m_textureBindSlots;
    std::vector<uint32> m_textureFreeBindSlots;

private:
    friend class R3_API Engine;
};

} // namespace R3
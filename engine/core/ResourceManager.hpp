#pragma once

#include <filesystem>
#include <span>
#include "api/Class.hpp"
#include "api/Types.hpp"
#include "render/Flags.hpp"
#include "render/RenderContext.hpp"
#include "render/ShaderObjects.hpp"

namespace R3 {

class ResourceManagerSingleton {
private:
    R3_CTOR_DEFAULT(ResourceManagerSingleton);
    R3_COPY_DELETE(ResourceManagerSingleton);
    R3_MOVE_DELETE(ResourceManagerSingleton);

public:
    usize createVertexBuffer(std::span<const Vertex> vertices);
    void* vertexBufferAt(usize index) noexcept;

    usize createIndexBuffer(std::span<const uint32> indices);
    void* indexBufferAt(usize index) noexcept;

    usize createTexture(const uint8* raw, usize width, usize height, TextureType type);
    usize createTexture(const uint8* compressed, usize size, TextureType type);
    usize createTexture(const std::filesystem::path& filepath, TextureType type);
    void* textureAt(usize index) noexcept;

private:
    // used by Engine to bind the render context
    void bindContext(IRenderContext* ctx) noexcept { m_ctx = ctx; }

    void free() noexcept;

private:
    IRenderContext* m_ctx = nullptr;

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
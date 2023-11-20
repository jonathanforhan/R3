#pragma once
#include <string_view>
#include "api/Types.hpp"

namespace R3 {

// clang-format off
enum class TextureType : uint8 {
    Albedo              = 0,
    Normal              = 1,
    MetallicRoughness   = 2,
    AmbientOcclusion    = 3,
    Emissive            = 4,
};
// clang-format on

class Texture2D {
public:
    Texture2D(std::string_view path, TextureType type);
    Texture2D(uint32 width, uint32 height, const void* data, TextureType type);
    Texture2D(const Texture2D&) = delete;
    Texture2D(Texture2D&& src) noexcept
        : m_id(src.m_id),
          m_type(src.m_type) {
        src.m_id = UINT32_MAX;
    }
    void operator=(const Texture2D&) = delete;
    Texture2D& operator=(Texture2D&& src) noexcept {
        m_id = src.m_id;
        m_type = src.m_type;
        src.m_id = UINT32_MAX;
        return *this;
    }
    ~Texture2D();

    void bind(uint8 index);
    auto type() const -> TextureType { return m_type; }
    auto typeBits() const -> uint8 { return static_cast<uint8>(m_type); }

private:
    uint32 m_id{0};
    TextureType m_type;
};

} // namespace R3
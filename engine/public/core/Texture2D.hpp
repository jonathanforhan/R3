#pragma once
#include <string_view>
#include "api/Types.hpp"

namespace R3 {

// clang-format off
enum class TextureType : uint8 {
    Undefined   = UINT8_MAX,
    Diffuse     = 0,
    Specular    = 1,
    Normals     = 2,
    Height      = 3,
    Emissive    = 4,
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
        src.m_id = 0;
    }
    void operator=(const Texture2D&) = delete;
    Texture2D& operator=(Texture2D&& src) noexcept {
        m_id = src.m_id;
        m_type = src.m_type;
        src.m_id = 0;
        src.m_type = TextureType::Undefined;
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

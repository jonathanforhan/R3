#pragma once
#include <string_view>
#include "api/Types.hpp"

namespace R3 {

// clang-format off
enum class TextureType : uint32{
    Undefined   = std::numeric_limits<uint32>::max(),
    Diffuse     = 0,
    Specular    = 1,
    Normals     = 2,
    Height      = 3,
    Emissive    = 4,
};
// clang-format on
struct TextureTypeBits {
    TextureTypeBits() = delete;
    static constexpr uint32 Undefined = static_cast<uint32>(TextureType::Undefined);
    static constexpr uint32 Diffuse = static_cast<uint32>(TextureType::Diffuse);
    static constexpr uint32 Specular = static_cast<uint32>(TextureType::Specular);
    static constexpr uint32 Normals = static_cast<uint32>(TextureType::Normals);
    static constexpr uint32 Height = static_cast<uint32>(TextureType::Height);
    static constexpr uint32 Emissive = static_cast<uint32>(TextureType::Emissive);
};

class Texture2D {
public:
    Texture2D(std::string_view path, TextureType type);
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
    auto typeBits() const -> uint32 { return static_cast<uint32>(m_type); }

private:
    uint32 m_id{0};
    TextureType m_type;
};

} // namespace R3

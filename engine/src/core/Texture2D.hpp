#pragma once
#include <string_view>
#include "api/Types.hpp"

namespace R3 {

enum class TextureType {
    Undefined = 0,
    Diffuse = 1,
    Specular = 2,
    Normals = 3,
    Height = 4,
    Emissive = 5,
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
        return *this;
    }
    ~Texture2D();

    void bind(uint8 index);
    auto type() const -> TextureType { return m_type; }

private:
    uint32 m_id{0};
    TextureType m_type;
};

} // namespace R3

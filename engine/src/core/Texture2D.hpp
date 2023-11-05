#pragma once
#include <string_view>
#include "api/Types.hpp"

namespace R3 {

class Texture2D {
public:
    Texture2D(std::string_view path);
    Texture2D(const Texture2D&) = delete;
    Texture2D(Texture2D&& src) noexcept
        : m_id(src.m_id) {
        src.m_id = 0;
    }
    void operator=(const Texture2D&) = delete;
    Texture2D& operator=(Texture2D&& src) noexcept {
        m_id = src.m_id;
        src.m_id = 0;
        return *this;
    }
    ~Texture2D();

    void bind(uint8 index);

private:
    uint32 m_id{0};
};

} // namespace R3

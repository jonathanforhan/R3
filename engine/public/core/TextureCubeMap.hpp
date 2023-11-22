#pragma once
#include "api/Types.hpp"
#include <string_view>

namespace R3 {

/// @brief Path of cube map components
struct TextureCubeMapCreateInfo {
    std::string_view right;  ///< Right face of cube map
    std::string_view left;   ///< Left face of cube map
    std::string_view top;    ///< Top face of cube map
    std::string_view bottom; ///< Bottom face of cube map
    std::string_view front;  ///< Front face of cube map
    std::string_view back;   ///< Back face of cube map
};

class TextureCubeMap {
public:
    /// @brief Create cube map texture
    /// @param createInfo paths of faces of the cube
    explicit TextureCubeMap(const TextureCubeMapCreateInfo& createInfo);

    TextureCubeMap(const TextureCubeMap&) = delete; ///< non-copyable

    /// @brief Moveable
    /// @param src 
    TextureCubeMap(TextureCubeMap&& src) noexcept
        : m_id(src.m_id) {
        src.m_id = 0;
    }

    void operator=(const TextureCubeMap&) = delete; ///< non-copyable

    /// @brief Movable
    /// @param src 
    /// @return moved TextureCubeMap
    TextureCubeMap& operator=(TextureCubeMap&& src) noexcept {
        m_id = src.m_id;
        src.m_id = 0;
        return *this;
    }

    ~TextureCubeMap(); ///< delete texture ID

    /// @brief Bind texture slot
    /// @param index texture slot 0-32 normally
    void bind(uint8 index);

private:
    uint32 m_id{0};
};

} // namespace R3

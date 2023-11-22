#pragma once
#include "api/Types.hpp"
#include "core/Renderbuffer.hpp"

namespace R3 {

/// @brief A collection of buffers than can be used as the destination for rendering
class Framebuffer {
protected:
    /// @brief Constructable by Renderer
    Framebuffer();
    friend class Renderer;

public:
    /// @brief Bind this Framebuffer
    void bind();

    /// @brief Bind default framebuffer as the Framebuffer
    static void bindDefault();

    /// @brief Bind a Renderbuffer to this Framebuffer
    /// @param renderbuffer buffer to bind
    void attachRenderBuffer(const Renderbuffer& renderbuffer);

private:
    uint32 m_id{0};
};

} // namespace R3
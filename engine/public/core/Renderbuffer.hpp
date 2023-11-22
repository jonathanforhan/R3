#pragma once
#include "api/Types.hpp"

namespace R3 {

/// @brief A data storage object containing a single image of a renderable internal format
class Renderbuffer {
protected:
    /// @brief Constructable by Renderer
    Renderbuffer();
    friend class Renderer;

public:
    /// @brief Query the Renderbuffer id
    /// @return buffer id
    uint32 bufferID() const { return m_id; }

private:
    uint32 m_id{0};
};

} // namespace R3

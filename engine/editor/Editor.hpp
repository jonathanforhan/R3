#if R3_EDITOR

#pragma once

#include "engine/api/Api.hpp"
#include "engine/render/CommandBuffer.hpp"

namespace R3 {

/// @brief Interface for implemented editor
class R3_API IEditor {
public:
    virtual ~IEditor() noexcept {};

    /// @brief Records the current editor UI frame
    /// @param dt The time elapsed since the last frame, in seconds.
    virtual void recordFrame(double dt) = 0;

    /// @brief Renderer specific draw call for the editor
    virtual void draw(ICommandBuffer& cmd) = 0;

    /// @brief True if the UI is currently focused (e.g. mouse over an UI element)
    virtual bool uiFocused() const = 0;

    /// @brief Get the currently selected entity ID in the editor, or 0xFFFF'FFFF if no entity is selected
    virtual uint32 selectedEntityID() const = 0;
};

} // namespace R3

#endif // R3_EDITOR
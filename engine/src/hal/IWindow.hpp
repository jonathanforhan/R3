#pragma once
#include "api/Types.hpp"

namespace R3 {

class IWindow {
protected:
    IWindow() = default;
    virtual void show() = 0;
    virtual void hide() = 0;
    virtual bool is_visible() const = 0;
    virtual void resize(int32 width, int32 height) = 0;
    virtual void size(int32* width, int32* height) const = 0;
    virtual float aspect_ratio() const = 0;
    virtual bool should_close() const = 0;
    virtual void update() = 0;
    virtual void* native_id() const = 0;
    virtual void* native_ptr() const = 0;
    virtual void kill() = 0;
};

} // namespace R3

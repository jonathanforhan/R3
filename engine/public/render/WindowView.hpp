#pragma once

#include <R3>

namespace R3 {

/// @brief Api facing view into the private Window class
class R3_API WindowView {
public:
    using WindowImpl = void*;

    DEFAULT_CONSTRUCT(WindowView);
    DEFAULT_COPY(WindowView);
    DEFAULT_MOVE(WindowView);

    WindowView(WindowImpl window);

    ivec2 size() const;

    float aspectRatio() const;

    WindowImpl handle() { return m_window; }

private:
    WindowImpl m_window;
};

} // namespace R3
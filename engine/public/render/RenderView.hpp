#pragma once

#include <R3>

namespace R3 {

/// @brief Public facing Render interface
class R3_API RenderView {
public:
    using RenderImpl = void*;

    DEFAULT_CONSTRUCT(RenderView);
    DEFAULT_COPY(RenderView);
    DEFAULT_MOVE(RenderView);

    RenderView(const RenderImpl renderer);

    void recreate();

    RenderImpl handle() { return m_renderer; }

private:
    RenderImpl m_renderer;
};

} // namespace R3
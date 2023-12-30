#include "RenderView.hpp"

#include "render/Renderer.hxx"

namespace R3 {

RenderView::RenderView(const RenderImpl renderer)
    : m_renderer(renderer) {
    CHECK(m_renderer != nullptr);
}

void RenderView::recreate() {
    reinterpret_cast<Renderer*>(m_renderer)->resize();
}

} // namespace R3
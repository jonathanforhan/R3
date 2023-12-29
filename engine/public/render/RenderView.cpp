#include "RenderView.hpp"

namespace R3 {

RenderView::RenderView(const RenderImpl renderer)
    : m_renderer(renderer) {
    CHECK(m_renderer != nullptr);
}

} // namespace R3
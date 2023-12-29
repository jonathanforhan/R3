#include "WindowView.hpp"

#include "render/Window.hxx"

namespace R3 {

WindowView::WindowView(WindowImpl window)
    : m_window(window) {
    CHECK(m_window != nullptr);
}

ivec2 WindowView::size() const {
    return reinterpret_cast<Window*>(m_window)->size();
}

float WindowView::aspectRatio() const {
    return reinterpret_cast<Window*>(m_window)->aspectRatio();
}

} // namespace R3
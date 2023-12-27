#pragma once

#include "core/Engine.hpp"
#include "ui/Widget.hpp"

namespace R3::ui {

class MainWindow : public Widget {
public:
    /// @brief Initialize new MainWindow
    /// @param title Window title
    MainWindow(const char* title = nullptr)
        : m_title(title),
          Widget(Engine::rootWidget()) {}

    /// @brief noop
    void draw() override {}

    /// @brief Extent getter
    uvec2 extent() const { return m_extent; }

    /// @brief Extent setter
    /// @param extent
    void setExtent(uvec2 extent) { m_extent = extent; }

private:
    const char* m_title;
    uvec2 m_extent;
};

} // namespace R3::ui
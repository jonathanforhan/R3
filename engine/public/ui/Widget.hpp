#pragma once

/// @file Widget.hpp
/// @brief Provides a Widget Base class for inherited widgets
/// This is trying to provide some of the ergonomics of QT without the hassle

#include <memory>
#include <vector>
#include "api/Construct.hpp"
#include "api/Ref.hpp"

namespace R3::ui {

/// @brief Widget Base class, each widget is a node in a tree
class Widget {
public:
    /// @brief List of automatically destroyed Widgets
    using WidgetList = std::vector<std::unique_ptr<Widget>>;

    /// @brief Construct Widget with a reference to it's parent
    /// @param parent
    Widget(Widget* parent = nullptr)
        : m_parent(parent) {}

    /// @brief Base class needs virtual destructor
    virtual ~Widget() {}

    /// @brief Draw capability to be set by children
    virtual void draw() = 0;

    /// @brief Set the parent widget, this can be done after creation
    /// @param parent
    void setParent(Ref<Widget> parent) { m_parent = parent; }

    /// @brief Get Parent of this widget
    /// @return parent pointer
    Ref<Widget> parent() const { return m_parent; }

    /// @brief Get children as a WidgetList
    /// @return children
    const WidgetList& children() { return m_children; };

    /// @brief Add a child to list, can be done this 'new' since they are store as unique ptrs
    /// @param child, most likely dynamically allocated
    void addChild(Widget* child) { m_children.emplace_back(child); }

    /// @brief Remove a child from child list by iterating through children O(n)
    /// @param child
    void removeChild(Widget* child) {
        auto it = std::find_if(m_children.begin(), m_children.end(), [child](auto& ch) { return ch.get() == child; });
        if (it != m_children.end())
            m_children.erase(it);
    }

private:
    Widget* m_parent;
    WidgetList m_children;
};

} // namespace R3::ui
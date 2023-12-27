#pragma once

#include "ui/Widget.hpp"

namespace R3::ui {

class Dock : public Widget {
public:
    Dock(Widget* parent = nullptr);

    void draw() override;
};

} // namespace R3::ui
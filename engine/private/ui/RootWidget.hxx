#pragma once

#include "ui/Widget.hpp"

namespace R3::ui {

class RootWidget : public Widget {
public:
    RootWidget()
        : Widget() {}

    void draw() override {}
};

} // namespace R3::ui
#pragma once

#include "ui/MainWindow.hpp"

namespace R3::editor {

class Editor : public ui::MainWindow {
public:
    NO_COPY(Editor);
    DEFAULT_MOVE(Editor);

    Editor();
    ~Editor();

private:
};

} // namespace R3::editor

#pragma once

#include "render/RenderFwd.hpp"

namespace R3::editor {

class Editor {
public:
    Editor();
    Editor(const Editor&) = delete;
    Editor(Editor&&) = delete;
    ~Editor();

private:
};

} // namespace R3::editor

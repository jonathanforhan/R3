#pragma once

#include <array>
#include <string_view>
#include "engine/api/Api.hpp"
#include "engine/api/Class.hpp"
#include "engine/api/Types.hpp"
#include "engine/input/InputCodes.hpp"

extern "C" struct R3_API GLFWwindow;

namespace R3 {

class R3_API Window {
private:
    R3_COPY_DELETE(Window);
    R3_MOVE_DELETE(Window);

    Window();

    ~Window() noexcept;

public:
    void setTitle(std::string_view title);

    void show();

    void hide();

    ivec2 size() const;

    int32 width() const;

    int32 height() const;

    ivec2 framebufferSize() const;

    int32 framebufferWidth() const;

    int32 framebufferHeight() const;

    void setSize(ivec2 size);

    void setSize(int32 width, int32 height);

    ivec2 position() const;

    void setPosition(ivec2 position);

    void setPosition(int32 x, int32 y);

    float aspectRatio() const;

    fvec2 contentScale() const;

    bool isMinimized() const;

    bool shouldClose() const;

    bool shouldResize() const;

    void setShouldResize(bool b);

    bool focused() const;

    bool uiFocused() const;

    bool keyPressed(Key key) const;

    bool mouseButtonPressed(MouseButton button) const;

    dvec2 cursorPosition() const;

    void* native();

    GLFWwindow* glfw();

    const GLFWwindow* glfw() const;

    void kill();

private:
    void update(bool uiFocused = false);

private:
    GLFWwindow* m_window                   = nullptr;
    bool m_shouldResize                    = false;
    bool m_uiFocused                       = false;
    static constexpr usize MAX_KEYS        = (uint16)Key::Menu - (uint16)Key::Space;
    std::array<bool, MAX_KEYS> m_keyStates = {};

private:
    friend class R3_API Engine;
};

} // namespace R3

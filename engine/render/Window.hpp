#pragma once

#include <string_view>
#include "Class.hpp"
#include "Types.hpp"

extern "C" struct GLFWwindow;

namespace R3 {

class Window {
public:
    R3_COPY_DELETE(Window);
    R3_MOVE_DELETE(Window);

    Window();

    ~Window() noexcept;

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

    void update();

    void* native();

    GLFWwindow* glfw();

    const GLFWwindow* glfw() const;

    void kill();

private:
    GLFWwindow* m_window = nullptr;
    bool m_shouldResize  = false;
};

} // namespace R3

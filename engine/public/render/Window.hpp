#pragma once

#include <api/Construct.hpp>
#include <api/Types.hpp>

extern "C" struct GLFWwindow; ///< Forward decl pointer needed for Window internals

namespace R3 {

/// @brief Window abstraction for a global OS window
/// Window must be initialized in main using Window::initialize(), if it is
/// called more than, nothing will happen. All function here must be called
/// from the main thread unless specified otherwise
class Window {
private:
    DEFAULT_CONSTRUCT(Window);

public:
    NO_COPY(Window);
    NO_MOVE(Window);

    /// @brief Get the global singleton Window instance
    /// @return Global instance
    static Window& instance();

    /// @brief Called once automatically by Engine::initialize
    /// @note Should be called ** Renderer::initialize
    void initialize();

    /// @brief Called once automatically by Engine::deinitialize
    /// Destroy Window and terminate glfw
    /// @note Should be called ** Renderer::initialize
    void deinitialize();

    /// @brief Show and focus Window
    void show();

    /// @brief Hide Window
    void hide();

    /// @brief Query extent of Window
    /// @return Size in screen coordinates x by y
    ivec2 size() const;

    /// @brief Query size of Window
    /// @param[out] width Window width in screen coordinates
    /// @param[out] height Window height in screen coordinates
    /// @note Either paramter CAN be NULL
    void size(int32* width, int32* height) const;

    /// @brief Query Window width
    /// @return Width in screen coordinates
    int32 width() const;

    /// @brief Query Window height
    /// @return Height in screen coordinates
    int32 height() const;

    /// @brief Resize Window
    /// @param extent Width by height window extent in screen coordinates
    /// @note On high DPI displays the pixels are scaled automatically
    void resize(ivec2 extent);

    /// @brief Resize Window
    /// @param width Window width in screen coordinates
    /// @param height Window height in screen coordinates
    /// @note On high DPI displays the pixels are scaled automatically
    void resize(int32 width, int32 height);

    /// @brief Query position of Window at upper left corner
    /// @return
    ivec2 position() const;

    /// @brief Query position of Window at upper left corner
    /// @param[out] x
    /// @param[out] y
    /// @note Either paramter CAN be NULL
    void position(int32* x, int32* y) const;

    /// @brief Set Window position
    /// @param position Screen coordinates of upper left corner
    void setPosition(ivec2 position);

    /// @brief Query position of Window
    /// @param[out] x X position of upper left corner
    /// @param[out] y Y position of upper left corner
    void setPosition(int32 x, int32 y);

    /// @brief Query Window aspect ratio
    /// @return Ratio width:height
    float aspectRatio() const;

    /// @brief Query content scale
    /// @return Ratio between current DPI and native DPI
    fvec2 contentScale() const;

    /// @brief Check whether Window is in minimized state
    /// @return State
    bool isMinimized() const;

    /// @brief Query close state of Window, can be set with Window::kill
    /// @return State
    bool shouldClose() const;

    /// @brief Set resize pending, used for swapchain recreation
    /// @param b State?
    void setShouldResize(bool b = true) { m_shouldResize = b; }

    /// @brief Query if resize is pending, used for swapchain recreatation
    /// @return Resize state
    bool shouldResize() const { return m_shouldResize; }

    /// @brief Update window by polling and (possibly) swapping buffers
    void update();

    /// @brief Query native OS window handle
    /// This is HWND on windows or Window (from X11Xlib.h) on linux
    /// @return Handle
    void* native();

    /// @brief Kill the Window, this changes to Window::shouldClose value
    void kill();

    /// @brief Get underlying GLFWwindow pointer
    /// @return pointer to GLFWwindow
    operator GLFWwindow*() { return m_window; }

private:
    GLFWwindow* m_window = nullptr;
    bool m_shouldResize  = false;
};

} // namespace R3

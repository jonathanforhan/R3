#pragma once

#include <atomic>
#include <string_view>

namespace R3 {

/// @brief Specification for Window creation
struct R3_API WindowSpecification {
    std::string_view title; ///< @brief Title shown on windowed bar
};

/// @brief Window class to abstract window operations
class R3_API Window : public NativeRenderObject {
public:
    NO_COPY(Window);
    NO_MOVE(Window);

    /// @brief Create Window from spec
    /// @param spec
    Window(const WindowSpecification& spec);

    /// @brief free Window
    ~Window();

    /// @brief Present Window
    void show();

    /// @brief Hide Window
    void hide();

    /// @brief Query whether Window is visible
    /// @return if window is visible
    [[nodiscard]] bool isVisible() const;

    /// @brief Resize the application Window
    /// @param extent width x height in pixels
    void resize(ivec2 extent);

    /// @brief Resize the application Window
    /// @param width width in pixels
    /// @param height height in pixels
    void resize(int32 width, int32 height);

    /// @brief Query the Window size
    /// @return Window dimensions in pixels
    [[nodiscard]] ivec2 size() const;

    /// @brief Query the Window size
    /// @param[out] width return the window width in pixels
    /// @param[out] height return the window height in pixels
    void size(int32& width, int32& height) const;

    /// @brief Query Window position in pixels
    /// @return position x by y
    [[nodiscard]] ivec2 position() const;

    /// @brief Query Window position in pixels
    /// @param x[out] return window width
    /// @param y[out] return window height
    void position(int32& x, int32& y) const;

    /// @brief Set Position of window in pixels
    /// @param position
    void setPosition(ivec2 position);

    /// @brief Set Position of window in pixels
    /// @param x
    /// @param y
    void setPosition(int32 x, int32 y);

    /// @brief Query the aspect ratio of the application window
    /// @return aspect ratio
    [[nodiscard]] float aspectRatio() const;

    /// @brief Query content scale
    /// @return a vec2 of scaleX and scaleY
    [[nodiscard]] vec2 contentScale() const;

    /// @brief Query the Window state for if it should close
    /// @return true if the window should close
    bool shouldClose() const;

    /// @brief Swapbuffers and poll events
    void update();

    /// @brief Query the Window's native ID
    /// @return native OS Window ID
    [[nodiscard]] void* nativeId() const;

    /// @brief Set the window should close to true
    void kill();

    /// @brief Query resize state
    /// @return true if needs to resize
    [[nodiscard]] bool shouldResize() const { return m_shouldResize; }

    /// @brief Set resize state
    /// @param b
    void setShouldResize(bool b) { m_shouldResize = b; }

private:
    std::atomic_bool m_shouldResize = false;
};

} // namespace R3

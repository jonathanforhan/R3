#pragma once

/// @file Window.hxx

#include <atomic>
#include <string_view>
#include "render/RenderApi.hxx"

namespace R3 {

/// @brief Specification for Window creation
struct WindowSpecification {
    std::string_view title; ///< @brief Title shown on windowed bar
};

/// @brief Window class to abstract window operations
class Window : public NativeRenderObject {
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
    bool isVisible() const;

    /// @brief Resize the application Window
    /// @param width width in pixels
    /// @param height height in pixels
    void resize(int32 width, int32 height);

    /// @brief Query the Window size
    /// @return Window dimensions in pixels
    ivec2 size() const;

    /// @brief Query the Window size
    /// @param[out] width return the window width in pixels
    /// @param[out] height return the window height in pixels
    void size(int32& width, int32& height) const;

    /// @brief Query the aspect ratio of the application window
    /// @return aspect ratio
    float aspectRatio() const;

    /// @brief Query the Window state for if it should close
    /// @return true if the window should close
    bool shouldClose() const;

    /// @brief Swapbuffers and poll events
    void update();

    /// @brief Query the Window's native ID
    /// @return native OS Window ID
    void* nativeId() const;

    /// @brief Set the window should close to true
    void kill();

    /// @brief Query resize state
    /// @return true if needs to resize
    bool shouldResize() const { return m_shouldResize; }

    /// @brief Set resize state
    /// @param b
    void setShouldResize(bool b) { m_shouldResize = b; }

private:
    std::atomic_bool m_shouldResize = false;
};

} // namespace R3
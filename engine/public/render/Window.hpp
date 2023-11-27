#pragma once
#include <string_view>
#include "api/Types.hpp"
#include "render/NativeRenderObject.hpp"

namespace R3 {

struct WindowSpecification {
    std::string_view title;
};

/// @brief Window class to abstract window operations
class Window : public NativeRenderObject {
public:
    void create(const WindowSpecification& spec);
    void destroy();

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
    std::tuple<int32, int32> size() const;

    /// @brief Query the Window size
    /// @param width return the window width in pixels
    /// @param height return the window height in pixels
    void size(int32& width, int32& height) const;

    /// @brief Query the aspect ratio of the application window
    /// @return aspect ratio
    float aspectRatio() const;

    /// @brief Query the Window state for if it should close
    /// @return true if the window should close
    bool shouldClose() const;

    /// @brief Query the Window state for if it should resize
    /// @return true if the window should resize
    bool shouldResize() const;

    /// @brief Set Window state, Window will set this automatically if it's framebuffer is resized
    /// @param b yes/no
    void setShouldResize(bool b);

    /// @brief Swapbuffers and poll events
    void update();

    /// @brief Query the Window's native ID
    /// @return native OS Window ID
    void* nativeId() const;

    /// @brief Set the window should close to true
    void kill();

private:
    WindowSpecification m_spec;

    bool m_shouldResize = false;
};

} // namespace R3
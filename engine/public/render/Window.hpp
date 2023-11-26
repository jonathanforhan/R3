#pragma once
#include <string_view>
#include "api/Types.hpp"
#include "render/NativeRenderObject.hpp"

namespace R3 {

/// @brief Window class to abstract window operations
class Window : public NativeRenderObject {
private:
    explicit Window(std::string_view title);
    friend class Engine;

public:
    Window(const Window&) = delete;
    void operator=(const Window&) = delete;
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

    /// @brief Swapbuffers and poll events
    void update();

    /// @brief Query the Window's native ID
    /// @return native OS Window ID
    void* nativeId() const;

    /// @brief Set the window should close to true
    void kill();
};

} // namespace R3
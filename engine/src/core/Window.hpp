#pragma once
#include <string_view>
#include "api/Types.hpp"

namespace R3 {

class Window {
private:
    explicit Window(std::string_view title);

public:
    Window(const Window&) = delete;
    void operator=(const Window&) = delete;
    ~Window();

    void show();
    void hide();
    auto isVisible() -> bool const;
    void resize(int32 width, int32 height);
    auto size() -> std::tuple<int32, int32> const;
    void size(int32& width, int32& height) const;
    auto aspectRatio() -> float const;
    auto shouldClose() -> bool const;
    void update();
    auto nativeId() -> void* const;
    auto nativeWindow() -> void* const;
    void kill();

private:
    void* m_pNativeWindow;

    friend class Engine;
};

} // namespace R3
#pragma once

#include <R3>
#include "render/Renderer.hpp"
#include "render/Window.hpp"

namespace R3 {

class R3_API Engine;
class R3_API Scene;

R3_EXPORT extern Engine* EngineInstance;
R3_EXPORT extern Scene* CurrentScene;

enum class R3_API EngineStatusCode {
    Success = 0,
    DlOutOfData = 1,
};

class R3_API Engine final {
public:
    HIDDEN_CONSTRUCT(Engine);

    [[nodiscard]] constexpr Window& window() { return m_window; }

    [[nodiscard]] constexpr Renderer& renderer() { return m_renderer; }

    [[nodiscard]] EngineStatusCode loop(const char* dlName);

    [[nodiscard]] double deltaTime();

private:
    Window m_window;
    Renderer m_renderer;
};

} // namespace R3

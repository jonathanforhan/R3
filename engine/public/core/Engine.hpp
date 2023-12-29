#pragma once

#include <R3>
#include "render/RenderView.hpp"
#include "render/WindowView.hpp"

namespace R3 {

class R3_API Engine;
class R3_API Scene;

R3_EXPORT extern Engine* EngineInstance;
R3_EXPORT extern Scene* CurrentScene;

class R3_API Engine final {
public:
    HIDDEN_CONSTRUCT(Engine);

    [[nodiscard]] WindowView& windowView() { return m_windowView; }

    [[nodiscard]] RenderView& renderView() { return m_renderView; }

    void loop();

    double deltaTime();

private:
    WindowView m_windowView;
    RenderView m_renderView;
};

} // namespace R3
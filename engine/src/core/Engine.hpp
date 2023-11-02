#pragma once
#if R3_OPENGL && (R3_VULKAN || R3_DX11 || R3_DX12)
#elif R3_VULKAN && (R3_DX11 || R3_DX12 || R3_OPENGL)
#elif R3_DX11 && (R3_DX12 || R3_OPENGL || R3_VULKAN)
#elif R3_DX12 && (R3_OPENGL || R3_VULKAN || R3_DX11)
#error "Multiple Renderers"
#endif

#include "api/Types.hpp"
#include "core/Renderer.hpp"
#include "core/Window.hpp"

namespace R3 {

class Engine {
private:
    Engine();

public:
    Engine(const Engine&) = delete;
    void operator=(const Engine&) = delete;

    static void initialize() { (void)instance(); }
    static Engine* const instance();
    void loop();
    void update();
    void draw_indexed(RendererPrimitive primitive, uint32 n_indices);

    Window* const window() { return &_window; }

public:
    mat4 view{mat4(1.0f)};
    mat4 projection{mat4(1.0f)};

private:
    Window _window;
    Renderer _renderer;
};

} // namespace R3

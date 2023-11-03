#pragma once
#if R3_OPENGL && (R3_VULKAN || R3_DX11 || R3_DX12)
#elif R3_VULKAN && (R3_DX11 || R3_DX12 || R3_OPENGL)
#elif R3_DX11 && (R3_DX12 || R3_OPENGL || R3_VULKAN)
#elif R3_DX12 && (R3_OPENGL || R3_VULKAN || R3_DX11)
#error "Multiple Renderers"
#endif

#include <functional>
#include <vector>
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
    static Engine& instance();
    void loop();
    template <typename Fn>
    void add_system(Fn&& fn) {
        _systems.emplace_back(fn);
    }
    void draw_indexed(RendererPrimitive primitive, uint32 n_indices);

    Window& window() { return _window; }

private:
    double delta_time() const;

public:
    mat4 view{mat4(1.0f)};
    mat4 projection{mat4(1.0f)};

    std::vector<std::function<void(double)>> _systems;

private:
    Window _window;
    Renderer _renderer;
};

} // namespace R3

#pragma once
#if R3_OPENGL && (R3_VULKAN || R3_DX11 || R3_DX12)
#elif R3_VULKAN && (R3_DX11 || R3_DX12 || R3_OPENGL)
#elif R3_DX11 && (R3_DX12 || R3_OPENGL || R3_VULKAN)
#elif R3_DX12 && (R3_OPENGL || R3_VULKAN || R3_DX11)
#error "Multiple Renderers"
#endif

#include <memory>
#include <unordered_map>
#include <vector>
#include "core/Camera.hpp"
#include "core/Component.hpp"
#include "core/Entity.hpp"
#include "core/Input.hpp"
#include "core/Mesh.hpp"
#include "core/Renderer.hpp"
#include "core/Shader.hpp"
#include "core/Texture2D.hpp"
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

    void add_entity(Entity* entity);

    void register_component(const std::string& name, Component* component);
    Component* component_ptr(const std::string& name) const;

    void register_mesh(const std::string& name, Mesh mesh);
    uint32 mesh_id(const std::string& name) const;

    void register_shader(const std::string& name, Shader shader);
    uint32 shader_id(const std::string& name) const;

    void register_texture2D(const std::string& name, Texture2D texture);
    uint32 texture2D_id(const std::string& name) const;

    bool mouse_down = false;
    struct cursor {
        float x, y;
    };
    cursor curr, prev;

public:
    mat4 view{mat4(1.0f)};
    mat4 projection{mat4(1.0f)};

private:
    Window _window;
    Renderer _renderer;
    Input _input;
    Camera _camera;
    std::vector<std::unique_ptr<Entity>> _entities;
    Record<std::unique_ptr<Component>> _components;
    Record<Mesh> _meshes;
    Record<Shader> _shaders;
    Record<Texture2D> _textures;
};

} // namespace R3

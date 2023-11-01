#include "Actor.hpp"
#include "core/Engine.hpp"

namespace R3 {

void Actor::set_mesh(const std::string& name) {
    if (uint32 id = Engine::instance()->mesh_id(name)) {
        _mesh_id = id;
    }
}

void Actor::set_shader(const std::string& name) {
    if (uint32 id = Engine::instance()->shader_id(name)) {
        _shader_id = id;
    }
}

void Actor::set_texture(const std::string& name) {
    if (uint32 id = Engine::instance()->texture2D_id(name)) {
        _texture_id = id;
    }
}

} // namespace R3

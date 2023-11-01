#pragma once
#include "api/Types.hpp"
#include "core/Entity.hpp"

namespace R3 {

class Actor : public Entity {
public:
    Actor() = default;
    virtual ~Actor() {}

    void set_mesh(const std::string& name);
    void set_shader(const std::string& name);
    void set_texture(const std::string& name);
    uint32 mesh_id() const { return _mesh_id; }
    uint32 shader_id() const { return _shader_id; }
    uint32 texture_id() const { return _texture_id; }

private:
    uint32 _mesh_id{0};
    uint32 _shader_id{0};
    uint32 _texture_id{0};
};

} // namespace R3

#pragma once
#include <entt/entt.hpp>

namespace R3 {

class EntityBase {
public:
    EntityBase() = default;
    EntityBase(entt::entity id)
        : _id(id) {}
    virtual ~EntityBase() {}

protected:
    entt::entity _id{entt::null};
};

} // namespace R3


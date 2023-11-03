#pragma once 
#include <entt/entt.hpp>
#include "api/Types.hpp"
#include "core/EntityBase.hpp"

namespace R3 {

class Scene {
private:
    Scene() = default;

public:
    Scene(const Scene&) = delete;
    void operator=(const Scene&) = delete;

    static Scene& instance();

private:
    entt::registry _registery;

    // Entity is a friend class to access _entities
    // all _entities management is done through Entity
    // this provides cleaner api at cost of being non-local
    friend class Entity;
};

} // namespace R3
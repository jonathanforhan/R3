#pragma once

#include "api/Types.hpp"
#include "core/Entity.hpp"

namespace R3 {

struct HierarchyComponent {
    Entity parent = entt::null;
    std::vector<Entity> children;
};

} // namespace R3

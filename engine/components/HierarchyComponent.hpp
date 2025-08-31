#pragma once

#include "engine/api/Api.hpp"
#include "engine/api/Types.hpp"
#include "engine/core/Entity.hpp"

namespace R3 {

struct R3_API HierarchyComponent {
    Entity parent = entt::null;
    std::vector<Entity> children;
};

} // namespace R3

#pragma once

#include <string>
#include "engine/core/Entity.hpp"

namespace R3 {

struct R3_API MetadataComponent {
    Entity entity = entt::null;
    std::string name;
};

} // namespace R3

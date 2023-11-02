#pragma once 
#include <deque>
#include <memory>
#include <vector>
#include <queue>
#include "api/Types.hpp"
#include "core/Entity.hpp"
#include "core/Component.hpp"

namespace R3 {

class Scene {
public:
    Scene() = default;
    Scene(const Scene&) = delete;
    void operator=(const Scene&) = delete;
    static Scene& instance();

private:
    std::deque<std::unique_ptr<Entity>> _entities;
};

} // namespace R3
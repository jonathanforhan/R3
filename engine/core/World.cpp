#include "World.hpp"

#include <memory>
#include "systems/System.hpp"

namespace R3 {

World& World::instance() {
    static World world;
    return world;
}

void World::update(double dt) {
    for (auto& system : m_systems) {
        system->update(dt);
    }
    m_camera.update(dt);
}

} // namespace R3
#include "World.hpp"

#include <memory>
#include "Camera.hpp"
#include "systems/System.hpp"

namespace R3 {

void World::update(double dt) {
    for (auto& system : m_systems) {
        system->update(dt);
    }
    m_camera.update(dt);
}

void World::clear() noexcept {
    m_registry.clear();
    m_systems.clear();
    m_systemSet.clear();
    m_componentFactoryMap.clear();
}

} // namespace R3
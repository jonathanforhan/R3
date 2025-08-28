#include "World.hpp"

#include <memory>
#include "Camera.hpp"
#include "systems/System.hpp"

namespace R3 {

void WorldSingleton::update(double dt) {
    for (auto& system : m_systems) {
        system->update(dt);
    }
    m_camera.update(dt);
}

} // namespace R3
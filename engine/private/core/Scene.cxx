#include "core/Scene.hpp"

#include "api/Memory.hxx"
#include "core/Engine.hpp"

namespace R3 {

Scene::Scene(uuid32 id)
    : id(id) {
    m_eventArena.reserve(KILOBYTE * 5);
}

} // namespace R3

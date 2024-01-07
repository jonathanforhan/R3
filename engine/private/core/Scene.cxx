#include "core/Scene.hpp"

#include "core/Entity.hpp"

namespace R3 {

Scene::Scene(uuid32 id, const char* name)
    : id(id),
      name(name) {
    static constexpr usize MB = 1024 * 1024;
    m_eventArena.reserve(MB);
}

} // namespace R3
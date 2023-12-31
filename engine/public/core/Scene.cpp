#include "Scene.hpp"

#include "Entity.hpp"
#include "render/ResourceManager.hxx"

namespace R3 {

Scene::Scene(uuid32 id)
    : id(id),
      resourceManager(new ResourceManager) {
    static constinit usize MB = 1024 * 1024;
    m_eventArena.reserve(MB);
}

R3::Scene::~Scene() {
    delete resourceManager;
}

} // namespace R3
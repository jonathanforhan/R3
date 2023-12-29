#include "Scene.hpp"

#include "render/ResourceManager.hxx"

namespace R3 {

Scene::Scene(uuid32 id)
    : id(id),
      resourceManager(new ResourceManager) {}

R3::Scene::~Scene() {
    delete resourceManager;
}

} // namespace R3
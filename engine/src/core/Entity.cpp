#include "Entity.hpp"
#include "core/Component.hpp"
#include "core/Engine.hpp"

namespace R3 {

void Entity::add_component(Component* component) {
    component->parent = this;
    component->initialize();
    component->parent = nullptr;
    _components.push_back(component);
}

void Entity::remove_component(Component* component) {
    auto it = std::ranges::find(_components, component);
    if (it != _components.end()) {
        (*it)->parent = this;
        (*it)->deinitialize();
        (*it)->parent = nullptr;
        _components.erase(it);
    }
}

} // namespace R3

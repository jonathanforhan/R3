#pragma once
#include <deque>
#include <memory>
#include <queue>
#include <vector>
#include "api/Check.hpp"
#include "api/Types.hpp"
#include "core/Component.hpp"
#include "core/Entity.hpp"

namespace R3 {

/* Entity Component System */
class ECS final {
private:
    ECS() = default;

public:
    ECS(const ECS&) = delete;
    void operator=(const ECS&) = delete;
    static ECS& instance();

    template <typename T, typename... Args>
    T& create_entity(Args&&... args);

    void destroy_entity(Entity& entity);

    template <typename T, typename... Args>
    void add_entity_component(Entity& entity, Args&&... args);

    template <typename T>
    T& get_entity_component(Entity& entity);

    template <typename T>
    std::vector<T>& components_by_type();

private:
    std::queue<usize> _entity_queue;
    std::deque<std::unique_ptr<Entity>> _entity_deque;

    std::vector<std::unique_ptr<ComponentBase>> _component_vec;
    std::unordered_map<std::string, usize> _component_map;
    std::unordered_map<uint32, std::unordered_map<usize, usize>> _ec_tracker;
};

template <typename T, typename... Args>
inline T& ECS::create_entity(Args&&... args) {
    static_assert(std::is_base_of<Entity, T>::value, "create_entity takes an Entity subclass");

    T* entity = new T(args...);

    if (_entity_queue.size()) {
        entity->_id = _entity_queue.back();
        _entity_queue.pop();
        _entity_deque[entity->_id].reset(entity);
    } else {
        entity->_id = _entity_deque.size();
        _entity_deque.emplace_back(entity);
    }

    return *entity;
}

inline void ECS::destroy_entity(Entity& entity) {
    _entity_deque[entity._id].reset();
    _entity_queue.push(entity._id);
}

template <typename T, typename... Args>
inline void ECS::add_entity_component(Entity& entity, Args&&... args) {
    if (!_component_map.contains(typeid(T).name())) {
        _component_map.emplace(typeid(T).name(), _component_vec.size());
        _component_vec.emplace_back(new Component<T>());
    }

    std::vector<T>& components = components_by_type<T>();
    usize component_index = _component_map[typeid(T).name()];
    usize nth_component = components.size();

    _ec_tracker[entity._id].emplace(component_index, nth_component);
    components.emplace_back(args...);
}

template <typename T>
inline T& ECS::get_entity_component(Entity& entity) {
    CHECK(_component_map.contains(typeid(T).name()));

    usize component_index = _component_map[typeid(T).name()];
    usize i = _ec_tracker[entity._id][component_index];

    return Component<T>::cast(_component_vec[component_index].get())->components[i];
}

template <typename T>
inline std::vector<T>& ECS::components_by_type() {
    return Component<T>::cast(_component_vec[_component_map[typeid(T).name()]].get())->components;
}

} // namespace R3
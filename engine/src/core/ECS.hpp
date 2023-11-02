#pragma once
#include <deque>
#include <memory>
#include <queue>
#include <vector>
#include "api/Check.hpp"
#include "api/Log.hpp"
#include "api/Types.hpp"
#include "core/Component.hpp"

namespace R3 {

using entity_t = uint64;

/* Entity Component System */
class ECS final {
private:
    ECS() = default;
    ECS(const ECS&) = delete;
    void operator=(const ECS&) = delete;
    static ECS& instance();

    entity_t create_entity();

    void destroy_entity(entity_t entity);

    template <typename T, typename... Args>
    void add_entity_component(entity_t entity, Args&&... args);

    template <typename T>
    bool has_entity_component(entity_t entity);

    template <typename T>
    T& get_entity_component(entity_t entity);

    template <typename T>
    std::vector<T>& get_components_by_type();

private:
    entity_t _entity_counter = 0;

    std::vector<std::unique_ptr<ComponentBase>> _component_vec;
    std::unordered_map<std::string, usize> _component_map;
    std::unordered_map<entity_t, std::unordered_map<usize, usize>> _ec_tracker;
    std::unordered_map<usize, usize> _free_slots;

    friend class Entity;
    friend class Scene;
};

inline entity_t ECS::create_entity() {
    return _entity_counter++;
}

inline void ECS::destroy_entity(entity_t entity) {
    for (auto& kv : _ec_tracker[entity]) {
        _free_slots[kv.first] = kv.second;
    }
}

template <typename T, typename... Args>
inline void ECS::add_entity_component(entity_t entity, Args&&... args) {
    if constexpr (!std::is_trivially_copyable<T>::value) {
        LOG(Warning, "Adding component", typeid(T).name(), "which is not trivially copyable");
    }

    if (!_component_map.contains(typeid(T).name())) {
        _component_map.emplace(typeid(T).name(), _component_vec.size());
        _component_vec.emplace_back(new Component<T>());
    }

    std::vector<T>& components = get_components_by_type<T>();
    usize component_index = _component_map[typeid(T).name()];

    if (_free_slots.contains(component_index)) {
        usize i = _free_slots[component_index];
        _ec_tracker[entity][component_index] = i;
        components[i] = std::move(T(args...));

        _free_slots.erase(_free_slots.find(component_index));
    } else {
        _ec_tracker[entity][component_index] = components.size();
        components.emplace_back(args...);
    }
}

template <typename T>
inline bool ECS::has_entity_component(entity_t entity) {
    usize component_index = _component_map[typeid(T).name()];
    return _ec_tracker[entity].contains(component_index);
}

template <typename T>
inline T& ECS::get_entity_component(entity_t entity) {
    CHECK(_component_map.contains(typeid(T).name()));

    usize component_index = _component_map[typeid(T).name()];
    usize i = _ec_tracker[entity][component_index];

    return Component<T>::cast(_component_vec[component_index].get())->components[i];
}

template <typename T>
inline std::vector<T>& ECS::get_components_by_type() {
    return Component<T>::cast(_component_vec[_component_map[typeid(T).name()]].get())->components;
}

} // namespace R3
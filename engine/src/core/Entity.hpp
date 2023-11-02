#pragma once
#include "api/Types.hpp"
#include "core/ECS.hpp"

namespace R3 {

class Entity {
public:
    Entity()
        : _id(ECS::instance().create_entity()) {}
    Entity(const Entity&) = delete;
    void operator=(const Entity&) = delete;
    virtual ~Entity() { ECS::instance().destroy_entity(_id); }

    virtual void tick(double){};

    template <typename T, typename... Args>
    void add_component(Args&&... args) {
        ECS::instance().add_entity_component<T>(_id, std::forward<Args>(args)...);
    }

    template <typename T>
    bool has_component() {
        return ECS::instance().has_entity_component<T>(_id);
    }

    template <typename T>
    T& get_component(entity_t entity) {
        return ECS::instance().get_entity_component<T>(_id);
    }

    template <typename T>
    static std::vector<T>& get_components_by_type() {
        return ECS::instance().get_components_by_type<T>();
    }

private:
    entity_t _id = 0;
};

} // namespace R3
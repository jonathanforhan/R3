/// @file TransformSystem.hpp

#pragma once

#include <entt/resource/resource.hpp>
#include "System.hpp"
#include "engine/api/Api.hpp"
#include "engine/api/Assert.hpp"
#include "engine/api/Exception.hpp"
#include "engine/api/Types.hpp"
#include "engine/components/HierarchyComponent.hpp"
#include "engine/components/TransformComponent.hpp"
#include "engine/core/Engine.hpp"
#include "engine/core/Entity.hpp"
#include "engine/core/World.hpp"

namespace R3 {

class R3_API TransformSystem : public ISystem {
public:
    virtual ~TransformSystem() noexcept override {}

    virtual void update(double dt) override {
        (void)dt;
        // update all transforms in hierarchy if parent transform changed
        // this has the limitation that if you change a child node you must inform the root that it's m_dirty
        GWorld()->registry().view<HierarchyComponent, TransformComponent>().each(
            [this](HierarchyComponent& hier, TransformComponent& t) {
                // if it's a root node (no parent) and has changed, update
                if (hier.parent == entt::null && t.m_dirty) {
                    for (Entity child : hier.children) {
                        // root node has no parent so local == world, we can just pass local transform to children
                        updateTransformHierarchy(child, t.local());
                    }
                    t.m_dirty = false;
                }
            });
    }

private:
    void updateTransformHierarchy(Entity ent, const dmat4& parentTransform) {
        TransformComponent& t = GWorld()->registry().get<TransformComponent>(ent);
        t.m_world             = parentTransform * t.local();
        t.m_dirty             = false;

        if (const HierarchyComponent* h = GWorld()->registry().try_get<HierarchyComponent>(ent)) {
            for (Entity child : h->children) {
                updateTransformHierarchy(child, t.world());
            }
        }
    }
};

} // namespace R3

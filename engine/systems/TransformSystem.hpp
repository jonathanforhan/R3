#pragma once

#include <entt/resource/resource.hpp>
#include "System.hpp"
#include "api/Assert.hpp"
#include "api/Exception.hpp"
#include "api/Types.hpp"
#include "components/HierarchyComponent.hpp"
#include "components/TransformComponent.hpp"
#include "core/Engine.hpp"
#include "core/Entity.hpp"
#include "core/World.hpp"

namespace R3 {

class TransformSystem : public ISystem {
public:
    virtual ~TransformSystem() noexcept override {}

    virtual void update(double dt) {
        (void)dt;
        // update all transforms in hierarchy if parent transform changed
        // this has the limitation that if you change a child node you must inform the root that it's m_dirty
        GWorld()->registry().view<HierarchyComponent, TransformComponent>().each(
            [this](HierarchyComponent& hier, TransformComponent& t) {
                // if it's a root node (no parent) and has changed, update
                if (hier.parent == entt::null && t.m_dirty) {
                    for (Entity child : hier.children) {
                        updateTransformHierarchy(child, t.transform());
                    }
                    t.m_dirty = false;
                }
            });
    }

private:
    void updateTransformHierarchy(Entity ent, const fmat4& parentTransform) {
        TransformComponent& t = GWorld()->registry().get<TransformComponent>(ent);
        t.transform()         = parentTransform * t.transform();
        t.m_dirty             = false;

        if (const HierarchyComponent* h = GWorld()->registry().try_get<HierarchyComponent>(ent)) {
            for (Entity child : h->children) {
                updateTransformHierarchy(child, t.transform());
            }
        }
    }
};

} // namespace R3

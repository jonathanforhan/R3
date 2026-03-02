#pragma once

#include "engine/api/Api.hpp"
#include "engine/api/Types.hpp"

namespace R3 {

class R3_API TransformComponent {
public:
    dmat4& transform() noexcept {
        m_dirty = true;
        return m_transform;
    }

    const dmat4& transform() const noexcept { return m_transform; }

private:
    dmat4 m_transform = dmat4(1.0);
    bool m_dirty      = true;

private:
    friend class R3_API TransformSystem;
};

} // namespace R3

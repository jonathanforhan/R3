#pragma once

#include "engine/api/Api.hpp"
#include "engine/api/Types.hpp"

namespace R3 {

class R3_API TransformComponent {
public:
    fmat4& transform() noexcept {
        m_dirty = true;
        return m_transform;
    }

    const fmat4& transform() const noexcept { return m_transform; }

private:
    fmat4 m_transform = fmat4(1.0f);
    bool m_dirty      = true;

private:
    friend class R3_API TransformSystem;
};

} // namespace R3

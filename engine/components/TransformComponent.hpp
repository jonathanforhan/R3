#pragma once

#include "api/Types.hpp"

namespace R3 {

class TransformComponent {
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
    friend class TransformSystem;
};

} // namespace R3

#pragma once

#include "engine/api/Api.hpp"
#include "engine/api/Types.hpp"

namespace R3 {

class R3_API TransformComponent {
public:
    const dmat4& local() const noexcept { return m_local; }

    dmat4& local() noexcept {
        m_dirty = true;
        return m_local;
    }

    const dmat4& world() const noexcept { return m_world; }

private:
    dmat4 m_local = dmat4(1.0);
    dmat4 m_world = dmat4(1.0);
    bool m_dirty  = true;

private:
    friend class R3_API TransformSystem;
};

} // namespace R3

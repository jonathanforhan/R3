#pragma once

#include "render/RenderContext.hpp"

namespace R3 {

class EngineSingleton final {
private:
    EngineSingleton() = default;

public:
    IRenderContext& context() noexcept { return *m_ctx; }

    int run();

private:
    double deltaTime();

private:
    IRenderContext* m_ctx = nullptr;
    bool m_running        = false;

private:
    friend struct Engine;
};

/// @brief Engine singleton instance.
struct Engine {
    EngineSingleton* operator->() noexcept {
        static EngineSingleton instance;
        return &instance;
    }
};

} // namespace R3

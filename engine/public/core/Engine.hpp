#pragma once

#include <api/Construct.hpp>

namespace R3 {

/// @brief R3 Engine singleton for high level management.
class Engine {
private:
    DEFAULT_CONSTRUCT(Engine);

public:
    NO_COPY(Engine);
    NO_MOVE(Engine);

    /// @brief Get the global singleton Window instance.
    /// @return Global instance
    static Engine& instance();

    /// @brief Called once (automatically) at the start of the program.
    /// Initialize Window, Renderer.
    void initialize();

    /// @brief Called once (automatically) at the end of the program.
    /// Deinitialize Window, Renderer.
    void deinitialize();

    int loop();
};

} // namespace R3

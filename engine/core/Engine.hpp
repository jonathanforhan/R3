#pragma once

namespace R3 {

class EngineSingleton final {
private:
    EngineSingleton() = default;

public:
    int run();

private:
    double deltaTime();

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

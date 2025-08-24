#pragma once

namespace R3 {

class Engine final {
private:
    Engine() = default;

public:
    static Engine& instance() noexcept;

    int run();

private:
    double deltaTime();
};

} // namespace R3

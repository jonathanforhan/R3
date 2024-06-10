#include "Application.hxx"

#include "core/Engine.hpp"

namespace R3 {

int Application::run() {
    auto& engine = Engine::instance();
    engine.initialize();
    int ret = engine.loop();
    engine.deinitialize();

    return ret;
}

} // namespace R3

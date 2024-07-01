#include "Application.hxx"

#include "core/Engine.hpp"
#include <api/Result.hpp>

namespace R3 {

Result<void> Application::run() {
    auto& engine = Engine::instance();
    engine.initialize();
    auto ret = engine.loop();
    engine.deinitialize();

    return ret;
}

} // namespace R3

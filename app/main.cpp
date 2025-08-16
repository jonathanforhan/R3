#include <exception>
#include "Engine.hpp"
#include "Exception.hpp"
#include "Log.hpp"

int main(int argc, char* argv[]) {
    using namespace R3;

#ifdef _WIN32
    detail::enableWindowsConsoleColors();
#endif

    int ret = -1;

    try {
        ret = Engine::instance().run();
    } catch (const Exception& ex) {
        LOG_ERROR("R3 Engine error: {}", ex.what());
        return -1;
    } catch (const std::exception& ex) {
        LOG_ERROR("Standard exception: {}", ex.what());
        return -1;
    }

    return ret;
}
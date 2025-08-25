#include <exception>
#include "api/Exception.hpp"
#include "core/Engine.hpp"
#include "core/Log.hpp"

using namespace R3;

int main(int argc, char* argv[]) {
    (void)argc, (void)argv;
#ifdef _WIN32
    detail::enableWindowsConsoleColors();
#endif

    int ret = -1;

    try {
        ret = Engine()->run();
    } catch (const Exception& ex) {
        LOG_ERROR("R3 Engine error: {}", ex.what());
        return -1;
    } catch (const std::exception& ex) {
        LOG_ERROR("Standard exception: {}", ex.what());
        return -1;
    }

    return ret;
}
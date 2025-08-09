#include <exception>
#include <Exception.hpp>
#include <Log.hpp>
#include <render/Renderer.hpp>
#include <render/Window.hpp>

using namespace R3;

int main() {
#ifdef _WIN32
    detail::enableWindowsConsoleColors();
#endif
    Window window;
    Renderer renderer(window);

    try {
        renderer.render();
    } catch (const Exception& ex) {
        LOG_ERROR("R3 Engine error: {}", ex.what());
        return -1;
    } catch (const std::exception& ex) {
        LOG_ERROR("Standard exception: {}", ex.what());
        return -1;
    }

    return 0;
}
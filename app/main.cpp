#include <chrono>
#include <exception>
#include <EventHandler.hpp>
#include <Exception.hpp>
#include <Log.hpp>
#include <Types.hpp>
#include <input/InputEvents.hpp>
#include <render/Renderer.hpp>
#include <render/Window.hpp>

using namespace R3;

static double deltaTime() {
    using namespace std::chrono;
    static auto s_prev = system_clock::now();
    auto now           = system_clock::now();
    double dt          = duration<double>(now - s_prev).count();
    s_prev             = now;
    return dt;
}

int main(int argc, char* argv[]) {
#ifdef _WIN32
    detail::enableWindowsConsoleColors();
#endif

    try {
        Window window;
        Renderer renderer(window);

        EventHandler::instance().bindEventListener("key-press"_event, [](const Event<KeyboardEventData>& event) {
            char c = (char)(event.data.key);
            LOG_INFO("pressed key: {}", c);
        });

        // Main render loop
        while (!window.shouldClose()) {
            double dt = deltaTime();

            window.update();

            usize remainingEvents;
            do {
                remainingEvents = EventHandler::instance().dispatchEvent();
            } while (remainingEvents > 0);

            renderer.render(dt);
        }
    } catch (const Exception& ex) {
        LOG_ERROR("R3 Engine error: {}", ex.what());
        return -1;
    } catch (const std::exception& ex) {
        LOG_ERROR("Standard exception: {}", ex.what());
        return -1;
    }

    return 0;
}
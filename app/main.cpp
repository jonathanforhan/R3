#include <exception>
#include <Exception.hpp>
#include <Log.hpp>
#include <media/glTF/glTF-Model.hpp>
#include <render/Renderer.hpp>
#include <render/Window.hpp>

using namespace R3;

int main() {
#ifdef _WIN32
    detail::enableWindowsConsoleColors();
#endif
    Window window;
    Renderer renderer(window);

    glTF::Model sponza{"assets/glTF-samples/Models/Sponza/glTF/Sponza.gltf"};
    glTF::Model helmet{"assets/glTF-samples/Models/DamagedHelmet/glTF/DamagedHelmet.gltf"};
    glTF::Model mosquito{"assets/glTF-samples/Models/MosquitoInAmber/glTF/MosquitoInAmber.gltf"};

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
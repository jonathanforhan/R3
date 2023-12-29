#include "Application.hxx"

#include <R3>
#include <R3_core>
#include <R3_systems>

namespace R3 {

Application::Application() {}

int Application::run() {
    EngineInstance = new Engine();

    {
        DynamicLibrary dl;
        DlEntry entry;
        DlRun run;

        try {
            dl.loadLib("vulkan.dll");

            entry = dl.loadEntry("VulkanEntry");
            CurrentScene = reinterpret_cast<Scene*>(entry());

            Scene::addSystem<CameraSystem>();

            run = dl.loadRunner("VulkanRun");
            run();
        } catch (std::exception& e) {
            LOG(Error, e.what());
            return EXIT_FAILURE;
        }

        EngineInstance->loop();
    }

    delete EngineInstance;

    return EXIT_SUCCESS;
}

} // namespace R3
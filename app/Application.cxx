#include "Application.hxx"

#include <R3>
#include <R3_core>
#include <R3_systems>
#include <filesystem>

static char const* ENTRY_TAG = "Entry";
static char const* RUN_TAG = "Run";
static char const* CLIENT_DL = "R3_client.dll";

namespace R3 {

int Application::run() {
    EngineInstance = new Engine();

    bool loop = true;

    while (loop) {
        if (std::filesystem::exists(USER_DL)) {
            std::filesystem::rename(USER_DL, CLIENT_DL);
        }

        DynamicLibrary dl;
        DlEntry entry;
        DlRun run;

        try {
            dl.loadLib(CLIENT_DL);

            entry = dl.loadEntry(ENTRY_TAG);
            CurrentScene = reinterpret_cast<Scene*>(entry());

            run = dl.loadRunner(RUN_TAG);
            run();
        } catch (std::exception& e) {
            LOG(Error, e.what());
            return EXIT_FAILURE;
        }

        auto ret = EngineInstance->loop(USER_DL);

        switch (ret) {
            case EngineStatusCode::DlOutOfData:
                // let the dl deconstructor call and load in the new one
                break;
            case EngineStatusCode::Success:
                // break loop and clean up
                loop = false;
                break;
        }

        EngineInstance->renderView().recreate();
    }

    delete EngineInstance;

    return EXIT_SUCCESS;
}

} // namespace R3
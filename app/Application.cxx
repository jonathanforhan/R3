#include "Application.hxx"

#include <R3>
#include <R3_core>
#include <R3_systems>
#include <filesystem>
#include "DynamicLibrary.hxx"

static char const* ENTRY_TAG = "Entry";
static char const* EXIT_TAG = "Exit";
static char const* RUN_TAG = "Run";
#define CLIENT_DL "libR3_client.so"

namespace R3 {

int Application::run() {
    EngineInstance = new Engine();

    bool loop = true;

    while (loop) {
        if (std::filesystem::exists(USER_DL)) {
            std::filesystem::rename(USER_DL, CLIENT_DL);
        } else {
            LOG(Warning, USER_DL, "not found");
        }

        DynamicLibrary dl;
        DlEntry dlEntry;
        DlExit dlExit;
        DlRun dlRun;

        try {
            dl.loadLib("./" CLIENT_DL);

            dlEntry = dl.loadEntry(ENTRY_TAG);
            CurrentScene = reinterpret_cast<Scene*>(dlEntry());

            dlExit = dl.loadExit(EXIT_TAG);

            dlRun = dl.loadRunner(RUN_TAG);
            dlRun();
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

        dlExit(CurrentScene);

        EngineInstance->renderer().recreate();
    }

    delete EngineInstance;

    return EXIT_SUCCESS;
}

} // namespace R3

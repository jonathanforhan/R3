#include "Application.hpp"

#include <cstdlib>
#include <exception>
#include <filesystem>
#include <new>
#include <editor/Editor.hpp>
#include <engine/api/Assert.hpp>
#include <engine/api/Exception.hpp>
#include <engine/core/Engine.hpp>
#include <engine/core/Log.hpp>
#include <engine/render/Window.hpp>
#include "DynamicLibrary.hpp"

static char const* ENTRY_TAG = "Entry";
static char const* EXIT_TAG  = "Exit";
static char const* LOOP_TAG  = "Loop";

namespace R3 {

int Application::run() {
#ifdef WIN32
    detail::enableWindowsConsoleColors();
#endif

    bool loop = true;

    DL_Entry dlEntry = nullptr;
    DL_Exit dlExit   = nullptr;
    DL_Loop dlLoop   = nullptr;
    DynamicLibrary dl;

    try {
        GEngine()->initialize();
        GEngine()->m_editor = new Editor(GEngine()->Window(), GEngine()->RenderContext());

        dl.loadLib("./sponza.dll");
        dlEntry = dl.loadEntry(ENTRY_TAG);
        dlExit  = dl.loadExit(EXIT_TAG);
        dlLoop  = dl.loadLoop(LOOP_TAG);
        R3_ASSERT(dlEntry && dlExit && dlLoop);

        dlEntry();

        while (loop && !GWindow()->shouldClose()) {
            GEngine()->update();
            dlLoop();
        }

        dlExit(nullptr);

    } catch (const Exception& ex) {
        LOG_ERROR("R3 Engine error: {}", ex.what());
        return EXIT_FAILURE;
    } catch (const std::exception& ex) {
        LOG_ERROR("Standard exception: {}", ex.what());
        return EXIT_FAILURE;
    }

    GEngine()->shutdown();

    return EXIT_SUCCESS;
}

} // namespace R3
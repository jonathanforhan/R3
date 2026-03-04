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

        // editor is created here due to no recursive linking between R3_ENGINE.dll and R3_EDITOR.dll
        GEngine()->m_editor = new Editor(GWindow(), GRenderContext());

        dl.loadLib("./sponza.dll");
        dlEntry = (DL_Entry)dl.loadFunc(ENTRY_TAG);
        dlExit  = (DL_Exit)dl.loadFunc(EXIT_TAG);
        dlLoop  = (DL_Loop)dl.loadFunc(LOOP_TAG);
        R3_ASSERT(dlEntry && dlExit && dlLoop);

        void* p = dlEntry();

        while (loop && !GWindow()->shouldClose()) {
            GEngine()->update();
            dlLoop();
        }

        dlExit(p);

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
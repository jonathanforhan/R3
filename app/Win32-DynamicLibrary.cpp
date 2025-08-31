#ifdef WIN32

#include <engine/api/Assert.hpp>
#include <engine/core/Log.hpp>
#include "DynamicLibrary.hpp"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

namespace R3 {

DynamicLibrary::~DynamicLibrary() {
    if (mod == NULL) {
        return;
    }

    // Windows DLLs are reference counted so if we free one in use somewhere else it's fine
    BOOL success = FreeLibrary((HMODULE)mod);

    mod = NULL;

    if (success != TRUE) {
        LOG_ERROR("failed to free a dynamic library");
    }
}

DL_Module DynamicLibrary::loadLib(char const* libpath) noexcept {
    mod = LoadLibraryExA(libpath, NULL, 0);
    return mod;
}

DL_Entry DynamicLibrary::loadEntry(char const* funcname) const noexcept {
    R3_ASSERT(mod != nullptr);
    DL_Entry entry = (DL_Entry)GetProcAddress((HMODULE)mod, funcname);
    return entry;
}

DL_Exit DynamicLibrary::loadExit(const char* funcname) const noexcept {
    R3_ASSERT(mod != nullptr);
    DL_Exit entry = (DL_Exit)GetProcAddress((HMODULE)mod, funcname);
    return entry;
}

DL_Loop DynamicLibrary::loadLoop(char const* funcname) const noexcept {
    R3_ASSERT(mod != nullptr);
    DL_Loop loop = (DL_Loop)GetProcAddress((HMODULE)mod, funcname);
    return loop;
}

} // namespace R3

#endif // WIN32
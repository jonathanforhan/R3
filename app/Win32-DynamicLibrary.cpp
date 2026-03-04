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

void* DynamicLibrary::loadFunc(char const* funcname) const noexcept {
    R3_ASSERT(mod != nullptr);
    void* entry = GetProcAddress((HMODULE)mod, funcname);
    return entry;
}

} // namespace R3

#endif // WIN32
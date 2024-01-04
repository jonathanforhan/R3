#include "DynamicLibrary.hxx"

namespace R3 {

DynamicLibrary::~DynamicLibrary() {
    if (mod == NULL) {
        LOG(Warning, "Attempting to free a NULL DL");
        return;
    }

    // Windows DLLs are reference counted so if we free one in use somewhere else it's fine
    BOOL success = FreeLibrary(mod);
    mod = NULL;

    if (success != TRUE) {
        LOG(Error, "failed to free a dynamic library");
    }
}

void DynamicLibrary::loadLib(char const* libpath) {
    mod = LoadLibraryExA(libpath, NULL, 0);
    CHECK(mod != nullptr);
}

DlEntry DynamicLibrary::loadEntry(char const* funcname) {
    CHECK(mod != nullptr);
    DlEntry entry = (DlEntry)GetProcAddress(mod, funcname);
    CHECK(entry != nullptr);
    return entry;
}

DlExit DynamicLibrary::loadExit(const char* funcname) {
    CHECK(mod != nullptr);
    DlExit entry = (DlExit)GetProcAddress(mod, funcname);
    CHECK(entry != nullptr);
    return entry;
}

DlRun DynamicLibrary::loadRunner(char const* funcname) {
    CHECK(mod != nullptr);
    DlRun run = (DlRun)GetProcAddress(mod, funcname);
    CHECK(run != nullptr);
    return run;
}

} // namespace R3
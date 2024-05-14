#ifdef __linux__

#include "DynamicLibrary.hxx"

#include <dlfcn.h>

namespace R3 {

DynamicLibrary::~DynamicLibrary() {
    if (mod == nullptr) {
        LOG(Warning, "Attempting to free a NULL DL");
        return;
    }

    // Windows DLLs are reference counted so if we free one in use somewhere else it's fine
    int success = dlclose(mod);
    mod = nullptr;

    if (success != 0) {
        LOG(Error, "failed to free a dynamic library");
    }
}

void DynamicLibrary::loadLib(char const* libpath) {
    mod = dlopen(libpath, RTLD_LAZY);
    CHECK(mod != nullptr);
}

DlEntry DynamicLibrary::loadEntry(char const* funcname) {
    CHECK(mod != nullptr);
    DlEntry entry = (DlEntry)dlsym(mod, funcname);
    CHECK(entry != nullptr);
    return entry;
}

DlExit DynamicLibrary::loadExit(const char* funcname) {
    CHECK(mod != nullptr);
    DlExit entry = (DlExit)dlsym(mod, funcname);
    CHECK(entry != nullptr);
    return entry;
}

DlRun DynamicLibrary::loadRunner(char const* funcname) {
    CHECK(mod != nullptr);
    DlRun run = (DlRun)dlsym(mod, funcname);
    CHECK(run != nullptr);
    return run;
}

} // namespace R3

#endif // WIN32

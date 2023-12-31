#pragma once

#include <R3>

#if _WIN32
#include <windows.h>
using DlModule = HMODULE;
#else
#error "Windows is currently the only platform supported"
#endif

using DlEntry = void*(__cdecl*)();

using DlRun = void(__cdecl*)();

namespace R3 {

struct DynamicLibrary {
    DynamicLibrary() = default;
    ~DynamicLibrary();

    void loadLib(const char* libpath);

    DlEntry loadEntry(const char* funcname);

    DlRun loadRunner(const char* funcname);

    DlModule mod = nullptr;
};

} // namespace R3
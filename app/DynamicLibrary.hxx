#pragma once

#include <windows.h>
#include <R3>

#if _WIN32
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

    void loadLib(char const* libpath);

    DlEntry loadEntry(char const* funcname);

    DlRun loadRunner(char const* funcname);

    DlModule mod = nullptr;
};

} // namespace R3
#pragma once

#include <R3>

using DlModule = void*;

using DlEntry = void*(__cdecl*)();

using DlExit = void(__cdecl*)(void*);

using DlRun = void(__cdecl*)();

namespace R3 {

struct DynamicLibrary {
    DynamicLibrary() = default;
    ~DynamicLibrary();

    void loadLib(const char* libpath);

    DlEntry loadEntry(const char* funcname);

    DlExit loadExit(const char* funcname);

    DlRun loadRunner(const char* funcname);

    DlModule mod = nullptr;
};

} // namespace R3

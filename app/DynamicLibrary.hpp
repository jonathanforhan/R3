#pragma once

using DL_Module = void*;
using DL_Entry  = void*(__cdecl*)();
using DL_Exit   = void(__cdecl*)(void*);
using DL_Loop   = void(__cdecl*)();

namespace R3 {

struct DynamicLibrary {
    DynamicLibrary() = default;
    ~DynamicLibrary();

    DL_Module loadLib(const char* libpath) noexcept;
    void* loadFunc(const char* funcname) const noexcept;

    DL_Module mod = nullptr;
};

} // namespace R3
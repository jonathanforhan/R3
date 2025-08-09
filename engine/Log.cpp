#include "Log.hpp"

#ifdef _WIN32

#include <io.h>
#include <windows.h>

namespace R3::detail {

void enableWindowsConsoleColors() noexcept {
    HANDLE hOut  = GetStdHandle(STD_OUTPUT_HANDLE);
    HANDLE hErr  = GetStdHandle(STD_ERROR_HANDLE);
    DWORD dwMode = 0;

    if (GetConsoleMode(hOut, &dwMode)) {
        SetConsoleMode(hOut, dwMode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
    }

    if (GetConsoleMode(hErr, &dwMode)) {
        SetConsoleMode(hErr, dwMode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
    }
}

} // namespace R3::detail

#endif

#pragma once

#ifndef R3_BUILD_DIST
#include <optick.h>
#endif

namespace R3 {

#ifndef R3_BUILD_DIST
#define R3_PROFILE(_Macro, ...) OPTICK_##_Macro(##__VA_ARGS__)
#else
#define R3_PROFILE(_X) void(0)
#endif

} // namespace R3
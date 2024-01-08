#pragma once

#include <R3>
#include "DynamicLibrary.hxx"

#define USER_DL "animation.dll"
// #define USER_DL "sponza.dll"

namespace R3 {

class Application {
public:
    int run();
};

} // namespace R3
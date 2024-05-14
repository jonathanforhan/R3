#pragma once

#include <R3>

// #define USER_DL "animation.dll"
#define USER_DL "libanimation.so"
// #define USER_DL "libsponza.so"

namespace R3 {

class Application {
public:
    int run();
};

} // namespace R3

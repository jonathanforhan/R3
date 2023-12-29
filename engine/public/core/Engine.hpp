#pragma once

#include <R3>

namespace R3 {

class R3_API Scene;

R3_EXPORT extern Scene* CurrentScene;

class R3_API Engine final {
public:
    Engine();
};

} // namespace R3
#pragma once
#include "systems/System.hpp"

namespace R3 {

class ModelSystem : public System {
public:
    void tick(double);
};
} // namespace R3
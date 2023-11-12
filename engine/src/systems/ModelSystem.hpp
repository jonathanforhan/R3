#pragma once
#include "systems/System.hpp"

namespace R3 {

class ModelSystem : public System {
public:
    void tick(double) override;
};

} // namespace R3
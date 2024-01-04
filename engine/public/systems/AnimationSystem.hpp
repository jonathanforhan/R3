#pragma once

#include "System.hpp"

namespace R3 {

class R3_API AnimationSystem : public System {
public:
    void tick(double dt) override;
};

} // namespace R3
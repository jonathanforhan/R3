#pragma once 

namespace R3 {

struct System {
    System() = default;
    virtual ~System() {}
    virtual void tick(double) = 0;
};

} // namespace R3
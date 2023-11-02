#pragma once
#include "api/Types.hpp"

namespace R3 {

class Entity {
public:
    Entity() = default;
    virtual ~Entity() {}

    virtual void tick(double){};

private:
    uint32 _id = 0;
};

} // namespace R3
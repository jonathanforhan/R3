#pragma once
#include "api/Types.hpp"

namespace R3 {

class Entity {
public:
    Entity()
        : _id(0) {}
    virtual ~Entity() {}
    virtual void tick(double){};

    uint64 id() const { return _id; };

private:
    uint64 _id;
};

} // namespace R3
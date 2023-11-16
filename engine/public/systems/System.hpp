/**
 * System is the base abstract class for Derived systems
 * they live in the Scene as a unique_ptrs and their virtual method
 * will be called each frame. The virtual overhead doesn't matter as
 * there will be few systems compared to Actors / Entities.
 */

#pragma once 

namespace R3 {

struct System {
    System() = default;
    virtual ~System() {}
    virtual void tick(double) = 0;
};

} // namespace R3
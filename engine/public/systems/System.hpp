#pragma once

namespace R3 {

/// @brief System is the base abstract class for Derived systems
/// they live in the Scene as a unique_ptrs and their virtual method
/// will be called each frame. The virtual overhead doesn't matter as
/// there will be few systems compared to Actors / Entities.
struct System {
    System() = default;

    /// @brief Base class virtual destructor
    virtual ~System() {}

    /// @brief Standard R3 tick method
    /// @param dt delta time in milliseconds
    virtual void tick(double dt) = 0;
};

} // namespace R3
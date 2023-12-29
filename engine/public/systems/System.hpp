#pragma once

/// @file System.hpp

namespace R3 {

/// @brief System is the base abstract class for Derived systems
/// they live in the Scene as a unique_ptrs and their virtual method
/// will be called each frame. The virtual overhead doesn't matter as
/// there will be few systems compared to Entities.
struct R3_API System {
    System() = default;

    /// @brief Base class virtual destructor
    virtual ~System() {}

    /// @brief Standard R3 tick method
    /// @param dt delta time in milliseconds
    virtual void tick(double dt) = 0;
};

/// @brief Ensure any system is derived from System and constructible with givene Args
template <typename T, typename... Args>
concept ValidSystem = std::is_base_of_v<System, T> and std::is_constructible_v<T, Args...>;

} // namespace R3
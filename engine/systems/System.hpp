#pragma once

#include <type_traits>

namespace R3 {

/// @brief ISystem is the interface class for Derived systems,
/// Their virtual update method will be called each frame.
/// The virtual overhead doesn't matter as there will be few systems compared to Entities.
class ISystem {
public:
    /// @brief Base class virtual destructor
    virtual ~ISystem() noexcept {}

    /// @brief Initializes the object (optional)
    // virtual void initialize() {}

    /// @brief Performs cleanup operations (optional)
    // virtual void cleanup() {}

    /// @brief Standard R3 update method
    /// @param dt delta time in milliseconds
    virtual void update(double dt) = 0;
};

/// @brief Ensure any system is derived from ISystem
template <typename T>
concept ValidSystem = std::is_base_of_v<ISystem, T>;

} // namespace R3
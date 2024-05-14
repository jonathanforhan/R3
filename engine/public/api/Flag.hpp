#pragma once

/// @file Flag.hpp

#include "Types.hpp"

namespace R3 {

/// @brief Flag ensures that flag structs are not constructable and provides Flags type
struct Flag {
    Flag() = delete;      ///< No Construction
    using Flags = uint32; ///< Flag type
};

} // namespace R3

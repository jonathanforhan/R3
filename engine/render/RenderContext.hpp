#pragma once

#include "api/Types.hpp"

namespace R3 {

/// @brief Interface for rendering contexts.
class IRenderContext {
public:
    R3_CTOR_DEFAULT(IRenderContext);
    R3_COPY_DELETE(IRenderContext);
    R3_MOVE_DEFAULT(IRenderContext);

    virtual ~IRenderContext() noexcept {};

    /// Returns the maximum number of frames that can be processed concurrently (in flight).
    virtual uint32 maxFramesInFlight() const noexcept = 0;
};

} // namespace R3
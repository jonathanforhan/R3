#pragma once

#include "api/Class.hpp"
#include "api/Types.hpp"

namespace R3 {

template <typename T>
concept RenderContextDerived = requires {
    { std::declval<const T>().maxFramesInFlight() } -> std::convertible_to<uint32>;
    { std::declval<T>().advanceFrame() } -> std::convertible_to<void>;
};

/// @brief Interface for rendering contexts.
class IRenderContext {
protected:
    R3_CTOR_DELETE(IRenderContext);
    R3_COPY_DELETE(IRenderContext);
    R3_MOVE_DEFAULT(IRenderContext);

    template <RenderContextDerived T>
    IRenderContext(std::type_identity<T>) {}

    virtual ~IRenderContext() noexcept {};
};

} // namespace R3
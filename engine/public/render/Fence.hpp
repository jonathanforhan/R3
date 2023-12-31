#pragma once

/// @brief Provides Host synchonization

#include "render/RenderApi.hpp"

namespace R3 {

/// @brief Fence Specification
struct R3_API FenceSpecification {
    const LogicalDevice& logicalDevice; ///< LogicalDevice
};

/// @brief Fences block Host operations and are used for synchronization
/// All Fences start signalled
class R3_API Fence : public NativeRenderObject {
public:
    DEFAULT_CONSTRUCT(Fence);
    NO_COPY(Fence);
    DEFAULT_MOVE(Fence);

    /// @brief Construct Fence from spec
    /// @param spec
    Fence(const FenceSpecification& spec);

    /// @brief Destroy Fence
    ~Fence();

    /// @brief Reset the signal of the fence
    void reset();

private:
    Ref<const LogicalDevice> m_logicalDevice;
};

} // namespace R3

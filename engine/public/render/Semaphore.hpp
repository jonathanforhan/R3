#pragma once

/// @brief Provides means of Device Synchronization

#include "render/RenderApi.hpp"

namespace R3 {

/// @brief Semaphore Specification
struct R3_API SemaphoreSpecification {
    const LogicalDevice& logicalDevice; ///< LogicalDevice
};

/// @brief Semaphore is a synchronization object that will block until a GPU action is completed
class R3_API Semaphore : public NativeRenderObject {
public:
    DEFAULT_CONSTRUCT(Semaphore);
    NO_COPY(Semaphore);
    DEFAULT_MOVE(Semaphore);

    /// @brief Construct Semaphore from spec
    /// @param spec
    Semaphore(const SemaphoreSpecification& spec);

    /// @brief Destroy Semaphore
    ~Semaphore();

private:
    Ref<const LogicalDevice> m_logicalDevice;
};

} // namespace R3

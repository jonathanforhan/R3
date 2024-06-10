#pragma once

#include "api/Construct.hpp"
#include <type_traits>

#if R3_VULKAN

namespace R3::vulkan {

/**
 * @brief Wrapper for C Vulkan objects that implicitly cast to underlying type
 * @tparam T Vulkan API Struct
 */
template <typename T>
requires std::is_pointer_v<T>
class VulkanObject {
public:
    using Handle = T; /**< Underlying Vulkan API Handle type */

protected:
    DEFAULT_CONSTRUCT(VulkanObject);
    NO_COPY(VulkanObject);

    /**
     * @brief Construct explicitly with handle
     * @param handle Vulkan Handle
     */
    VulkanObject(T handle)
        : m_handle(handle) {}

    /**
     * @brief Move constructor that invalidates moved object's handle
     * @param other Moved object
     */
    VulkanObject(VulkanObject&& other) noexcept
        : m_handle(other.m_handle) {
        other.m_handle = nullptr;
    }

    /**
     * @brief Move assignment operator that invalidates moved object's handle
     * @param other Moved object
     * @return This
     */
    VulkanObject& operator=(VulkanObject&& other) noexcept {
        m_handle       = other.m_handle;
        other.m_handle = nullptr;
        return *this;
    }

public:
    /**
     * @brief Cast to Underlying Vulkan C Api Handle
     */
    operator Handle() { return m_handle; }

    /**
     * @brief Const cast to Underlying Vulkan C Api Handle
     */
    operator const Handle() const { return m_handle; }

protected:
    Handle m_handle = nullptr; /**< Underlying Handle e.g. VkInstance, VkDevice, etc */
};

} // namespace R3::vulkan

#endif // R3_VULKAN
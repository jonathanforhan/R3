/**
 * @file vulkan-VulkanObject.hxx
 * @copyright GNU Public License
 */

#pragma once

#if R3_VULKAN

#include <api/Construct.hpp>
#include <type_traits>

namespace R3::vulkan {

/**
 * Wrapper for C Vulkan objects, handles move construction for RAII.
 * If inheriting from VulkanObject multiple times it is recommended to define
 * which VulkanObject<T>::vk the class is using.
 * @code
 * class VulkanObjectChild : public VulkanObject<Foo>, public VulkanObject<Bar> {
 * public:
 *     using VulkanObject<Foo>::vk;
 * };
 * @endcode
 * @tparam T Vulkan API Handle
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
     * Construct with handle.
     * @param handle Vulkan API Handle
     */
    VulkanObject(T handle)
        : m_handle(handle) {}

    /**
     * Move constructor that invalidates moved object's handle.
     * @param other Moved object
     */
    VulkanObject(VulkanObject&& other) noexcept
        : m_handle(other.m_handle) {
        other.m_handle = nullptr;
    }

    /**
     * Move assignment operator that invalidates moved object's handle.
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
     * Get underlying Vulkan API Handle.
     * @return Vulkan API Handle
     */
    constexpr Handle vk() { return m_handle; }

    /**
     * Get underlying const Vulkan API Handle.
     * @return Const Vulkan API Handle
     */
    constexpr Handle vk() const { return m_handle; }

protected:
    Handle m_handle = nullptr; /**< Underlying Handle e.g. VkInstance, VkDevice, etc */
};

} // namespace R3::vulkan

#endif // R3_VULKAN
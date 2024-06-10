#pragma once

#if R3_VULKAN

#include "api/Construct.hpp"
#include "api/Types.hpp"
#include "vulkan-fwd.hxx"
#include "vulkan-VulkanObject.hxx"
#include <span>
#include <vector>
#include <vulkan/vulkan.h>

namespace R3::vulkan {

/**
 * @brief Vulkan PhysicalDevice Specification
 */
struct PhysicalDeviceSpecification {
    const Instance& instance;                   /**< Valid Vulkan Instance */
    const Surface& surface;                     /**< Valid OS Surface */
    const std::vector<const char*>& extensions; /**< PhysicalDevice extensions required */
};

/**
 * @brief Vulkan PhysicalDevice RAII wrapper
 *
 * After initializing a VkInstance we can now look for a graphics card, this is
 * our VkPhysicalDevice. This class provides means of checking for support and
 * evalutaing which graphics card is the best fit for the application. R3 does
 * not support usage of multiple graphics cards at once (at an API level).
 */
class PhysicalDevice : public VulkanObject<VkPhysicalDevice> {
public:
    DEFAULT_CONSTRUCT(PhysicalDevice);
    NO_COPY(PhysicalDevice);
    DEFAULT_MOVE(PhysicalDevice);

    /**
     * @brief Automatically find the most suitable graphics card and select it
     *
     * Ranks graphics cards, assigning them and score and chooses the card with
     * the best score. Then stores the sample rate of the card for query.
     *
     * @param spec
     */
    PhysicalDevice(const PhysicalDeviceSpecification& spec);

    /**
     * @brief Get the memory type index from PhysicalDevice
     *
     * Memory type index identifies where certain type of memory is stored
     * on the GPU. This function is used in the creatation of Buffers.
     *
     * @code
     *
     * VkMemoryRequirements requirements;
     * vkGetBufferMemoryRequirements(device, buffer, &requirements);
     *
     * uint32 memoryTypeIndex = physicalDevice.queryMemoryType(
     *   requirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
     *
     * @endcode
     *
     * @param typeFilter This is given by VkMemoryRequirements::memoryTypeBits
     * @param propertyFlags Flags determining memory location and coherency
     * @return memory type index
     */
    uint32 queryMemoryType(uint32 typeFilter, VkMemoryPropertyFlags propertyFlags) const;

    /**
     * @brief Get stored PhyscialDevice extensions
     * @return Extensions
     */
    std::span<const char* const> extensions() const { return m_extensions; }

    /**
     * @brief Highest sample count supported for color and depth framebuffers
     * @return Sample count per pixel
     */
    constexpr VkSampleCountFlagBits sampleCount() const { return static_cast<VkSampleCountFlagBits>(m_sampleCount); }

private:
    // Rank GPU on several factors to determine best device to run on
    int32 evaluateDevice(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface) const;

    // Ensures all required extensions are preset on physical device
    static bool checkExtensionSupport(VkPhysicalDevice physicalDevice, std::span<const char* const> extensions);

private:
    std::vector<const char*> m_extensions;
    uint8 m_sampleCount = undefined;
};

} // namespace R3::vulkan

#endif // R3_VULKAN
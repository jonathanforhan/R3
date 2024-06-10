#pragma once

#if R3_VULKAN

#define R3_VALIDATION_LAYERS (not R3_BUILD_DISTRIBUTION)

#include "api/Construct.hpp"
#include "vulkan-VulkanObject.hxx"
#include <span>
#include <vector>
#include <vulkan/vulkan.h>

namespace R3::vulkan {

/**
 * @brief Vulkan Instance Specification
 */
struct InstanceSpecification {
    const char* applicationName;                   /**< Name of application */
    std::span<const char* const> extensions;       /**< Vulkan extensions */
    std::span<const char* const> validationLayers; /**< Vulkan validation layers */
};

/**
 * @brief Vulkan Instance RAII wrapper
 *
 * A Vulkan Instance is the connection between a user applcation and
 * the graphics driver.
 */
class Instance : public VulkanObject<VkInstance> {
public:
    DEFAULT_CONSTRUCT(Instance);
    NO_COPY(Instance);
    DEFAULT_MOVE(Instance);

    /**
     * @brief Create Vulkan Instance stored as VulkanObject::Handle
     *
     * Checks extension and validation support in DEBUG build, adds validation
     * layers if R3_VALIDATION_LAYERS is true and sets up debug logging. Always
     * ensures VK_SUCCESS on creatation.
     *
     * @param spec
     */
    Instance(const InstanceSpecification& spec);

    /**
     * @brief Destroy Vulkan Instance calling vkDestroyInstance
     *
     * All Vulkan objects that depend on the instance must be destroyed before
     * the Instance is destroyed.
     */
    ~Instance();

    /**
     * @brief Query required instance extensions from GLFW API
     * @return List of required extensions
     */
    static std::vector<const char*> queryRequiredExtensions();

private:
    bool checkExtensionSupport(std::span<const char* const> requiredExtensions) const;

    bool checkValidationLayerSupport(std::span<const char* const> requiredValidationLayers) const;
};

} // namespace R3::vulkan

#endif // R3_VULKAN
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file vulkan-Instance.hxx
/// @copyright GNU Public License
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#if R3_VULKAN

#include "vulkan-VulkanObject.hxx"
#include <api/Construct.hpp>
#include <span>
#include <vector>
#include <vulkan/vulkan.h>

/// @def R3_VALIDATION_LAYERS
/// @brief Determines whether the vulkan instance enables validation layers.
#define R3_VALIDATION_LAYERS (not R3_BUILD_DISTRIBUTION)

namespace R3::vulkan {

/// @brief Vulkan Instance Specification.
struct InstanceSpecification {
    const char* applicationName;                   ///< Name of application.
    std::span<const char* const> extensions;       ///< Vulkan instance extensions.
    std::span<const char* const> validationLayers; ///< Vulkan validation layers.
};

/// @brief Vulkan Instance RAII wrapper.
/// https://registry.khronos.org/vulkan/specs/1.3-extensions/man/html/VkInstance.html
class Instance : public VulkanObject<VkInstance> {
public:
    DEFAULT_CONSTRUCT(Instance);
    NO_COPY(Instance);
    DEFAULT_MOVE(Instance);

    /// @brief Create Vulkan Instance stored as VulkanObject::Handle.
    /// Checks extension and validation support in DEBUG build, adds validation layers if R3_VALIDATION_LAYERS is true
    /// and sets up debug logging. Always ensures VK_SUCCESS on creatation.
    /// @param spec
    Instance(const InstanceSpecification& spec);

    /// @brief Destroy Vulkan Instance calling vkDestroyInstance.
    /// All Vulkan objects that depend on the instance must be destroyed before the Instance is destroyed.
    ~Instance();

    /// @brief Query required instance extensions from GLFW API.
    /// @return List of required extensions
    static std::vector<const char*> queryRequiredExtensions();

private:
    bool checkExtensionSupport(std::span<const char* const> requiredExtensions) const;

    bool checkValidationLayerSupport(std::span<const char* const> requiredValidationLayers) const;
};

} // namespace R3::vulkan

#endif // R3_VULKAN
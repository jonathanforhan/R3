#pragma once

#include <vector>
#include "render/NativeRenderObject.hpp"

#define R3_VALIDATION_LAYERS_ENABLED R3_BUILD_DEBUG

namespace R3 {

/// @brief Instance Specification
struct InstanceSpecification {
    const char* applicationName;               ///< @brief Name of application
    std::vector<const char*> extensions;       ///< @brief Renderer extensions
    std::vector<const char*> validationLayers; ///< @brief Renderer validation layers
};

/// @brief Many graphics APIs have a concept of instance, or per-application state, this wraps those instances
class Instance : public NativeRenderObject {
public:
    /// @brief Create Instance, this is where instance extensions and validation is setup
    /// @param spec
    void create(const InstanceSpecification& spec);

    /// @brief Free Instance
    void destroy();

private:
    bool checkExtensionSupport(const std::vector<const char*>& requiredExtensions) const;
    bool checkValidationLayerSupport(const std::vector<const char*>& requiredValidationLayers) const;

private:
    InstanceSpecification m_spec;
};

} // namespace R3

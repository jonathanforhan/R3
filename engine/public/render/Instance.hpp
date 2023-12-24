#pragma once

/// @file Instance.hpp

#include <span>
#include <string_view>
#include "render/RenderApi.hpp"

/// @def R3_VALIDATION_LAYERS_ENABLED
/// Validation Status (only debug)
#define R3_VALIDATION_LAYERS_ENABLED R3_BUILD_DEBUG

namespace R3 {

/// @brief Instance Specification
struct InstanceSpecification {
    std::string_view applicationName;        ///< Name of application
    std::span<const char*> extensions;       ///< Renderer extensions
    std::span<const char*> validationLayers; ///< Renderer validation layers
};

/// @brief Many graphics APIs have a concept of instance, or per-application state, this wraps those instances
class Instance : public NativeRenderObject {
public:
    DEFAULT_CONSTRUCT(Instance);
    NO_COPY(Instance);
    DEFAULT_MOVE(Instance);

    /// @brief Initialize instance from spec
    /// @param spec
    Instance(const InstanceSpecification& spec);

    /// @brief free instance
    ~Instance();

private:
    bool checkExtensionSupport(std::span<const char*> requiredExtensions) const;
    bool checkValidationLayerSupport(std::span<const char*> requiredValidationLayers) const;
};

} // namespace R3

#pragma once

#include <span>
#include <string_view>
#include <vector>
#include "render/NativeRenderObject.hpp"

#define R3_VALIDATION_LAYERS_ENABLED R3_BUILD_DEBUG

namespace R3 {

/// @brief Instance Specification
struct InstanceSpecification {
    std::string_view applicationName;          ///< @brief Name of application
    std::vector<const char*> extensions;       ///< @brief Renderer extensions
    std::vector<const char*> validationLayers; ///< @brief Renderer validation layers
};

/// @brief Many graphics APIs have a concept of instance, or per-application state, this wraps those instances
class Instance : public NativeRenderObject {
public:
    Instance() = default;

    /// @brief Initialize instance from spec
    /// @param spec 
    Instance(const InstanceSpecification& spec);

    Instance(Instance&&) noexcept = default;
    Instance& operator=(Instance&&) noexcept = default;

    /// @brief free instance
    ~Instance();

private:
    bool checkExtensionSupport(std::span<const char*> requiredExtensions) const;
    bool checkValidationLayerSupport(std::span<const char*> requiredValidationLayers) const;

private:
    InstanceSpecification m_spec;
};

} // namespace R3

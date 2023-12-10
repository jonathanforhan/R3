#pragma once

#include <span>
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
    Instance() = default;
    Instance(const InstanceSpecification& spec);
    Instance(Instance&&) noexcept = default;
    Instance& operator=(Instance&&) noexcept = default;
    ~Instance();

private:
    bool checkExtensionSupport(std::span<const char*> requiredExtensions) const;
    bool checkValidationLayerSupport(std::span<const char*> requiredValidationLayers) const;

private:
    InstanceSpecification m_spec;
};

} // namespace R3

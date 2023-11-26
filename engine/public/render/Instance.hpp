#pragma once
#include <vector>
#include "render/NativeRenderObject.hpp"

#define R3_VALIDATION_LAYERS_ENABLED R3_BUILD_DEBUG

namespace R3 {

struct InstanceSpecification {
    const char* applicationName;
    std::vector<const char*> extensions;
    std::vector<const char*> validationLayers;
};

class Instance : public NativeRenderObject {
public:
    void create(const InstanceSpecification& spec);
    void destroy();

private:
    bool checkExtensionSupport(const std::vector<const char*>& requiredExtensions) const;
    bool checkValidationLayerSupport(const std::vector<const char*>& requiredValidationLayers) const;

private:
    InstanceSpecification m_spec;
};

} // namespace R3

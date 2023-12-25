#pragma once

#include "api/Types.hpp"

#define MAX_LIGHTS 1

namespace R3::vulkan {

// Layout of vulkan UBO aligned
struct UniformBufferObject {
    alignas(16) mat4 model;
};

struct PointLight {
    alignas(16) vec3 position;
    alignas(16) vec3 color;
    alignas(16) vec3 intensity;
};

struct LightBufferObject {
    alignas(16) vec3 cameraPosition;
    PointLight pointLights[1];
    alignas(4) uint32 lightCount;
    alignas(4) uint32 pbrFlags;
};

} // namespace R3::vulkan
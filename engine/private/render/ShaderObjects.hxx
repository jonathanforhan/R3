#pragma once
#pragma warning(push)
#pragma warning(disable : 4324)

#include "api/Types.hpp"

namespace R3 {

static constexpr uint32 MAX_LIGHTS = 32;

struct VertexUniformBufferObject {
    alignas(16) mat4 model;
};

struct ViewProjection {
    alignas(16) mat4 view;
    alignas(16) mat4 projection;
};

struct VertexPushConstant {
    ViewProjection viewProjection;
};

struct PointLightShaderObject {
    alignas(16) vec3 position;
    alignas(16) vec3 color;
    alignas(4) float intensity;
};

struct FragmentUniformBufferObject {
    alignas(16) vec3 cameraPosition;
    alignas(4) uint32 pbrFlags;
    alignas(4) uint32 lightCount;
    PointLightShaderObject pointLights[MAX_LIGHTS];
};

} // namespace R3

#pragma warning(pop)
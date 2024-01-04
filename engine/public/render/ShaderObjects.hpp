#pragma once
#pragma warning(push)
#pragma warning(disable : 4324)

#include "api/Types.hpp"

namespace R3 {

static constexpr uint32 MAX_LIGHTS = 128;

struct R3_API ViewProjection {
    alignas(16) mat4 view;
    alignas(16) mat4 projection;
};

struct R3_API VertexUniformBufferObject {
    alignas(16) mat4 model;
    alignas(16) mat4 view;
    alignas(16) mat4 projection;
};

struct R3_API FragmentPushConstant {
    alignas(8) vec2 cursorPosition;
    alignas(4) uint32 uid;
    alignas(4) uint32 selected;
};

struct R3_API PointLightShaderObject {
    alignas(16) vec3 position;
    alignas(16) vec3 color;
    alignas(4) float intensity;
};

struct R3_API FragmentUniformBufferObject {
    alignas(16) vec3 cameraPosition;
    alignas(4) uint32 pbrFlags;
    alignas(4) uint32 lightCount;
    PointLightShaderObject pointLights[MAX_LIGHTS];
};

} // namespace R3

#pragma warning(pop)
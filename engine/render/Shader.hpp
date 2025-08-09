#pragma once

#include <cstdint>
#include <span>
#include <string>
#include <vector>
#include <vulkan/vulkan_core.h>
#include "Types.hpp"

namespace R3 {

class RenderContext;

struct ShaderStageFlags {
    enum : uint32 {
        Vertex                 = 0x00000001,
        TessellationControl    = 0x00000002,
        TessellationEvaluation = 0x00000004,
        Geometry               = 0x00000008,
        Fragment               = 0x00000010,
        Compute                = 0x00000020,
        AllGraphics            = 0x0000001f,
        All                    = 0x7fffffff,
        RaygenKhr              = 0x00000100,
        AnyHitKhr              = 0x00000200,
        ClosestHitKhr          = 0x00000400,
        MissKhr                = 0x00000800,
        IntersectionKhr        = 0x00001000,
        CallableKhr            = 0x00002000,
        TaskExt                = 0x00000040,
        MeshExt                = 0x00000080,
        SubpassShadingHuawei   = 0x00004000,
        ClusterCullingHuawei   = 0x00080000,
        RaygenNv               = RaygenKhr,
        AnyHitNv               = AnyHitKhr,
        ClosestHitNv           = ClosestHitKhr,
        MissNv                 = MissKhr,
        IntersectionNv         = IntersectionKhr,
        CallableNv             = CallableKhr,
        TaskNv                 = TaskExt,
        MeshNv                 = MeshExt,
    };
};
using ShaderStage = uint32;

class Shader {
public:
    void createFromFile(RenderContext& ctx, const std::string& filename, ShaderStage type);

    void createFromSource(RenderContext& ctx, std::span<const uint32_t> spirvCode, ShaderStage type);

    void destroy() noexcept;

    VkShaderModule handle() noexcept { return m_shaderModule; }

    ShaderStage type() const noexcept { return m_type; }

    bool isValid() const noexcept { return m_shaderModule != VK_NULL_HANDLE; }

private:
    std::vector<uint32_t> readFile(const std::string& filename) const;

private:
    VkDevice m_device             = VK_NULL_HANDLE;
    VkShaderModule m_shaderModule = VK_NULL_HANDLE;
    ShaderStage m_type            = ShaderStageFlags::Vertex;
};

} // namespace R3
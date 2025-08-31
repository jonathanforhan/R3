#pragma once

#include "engine/api/Api.hpp"
#include "engine/api/Types.hpp"

namespace R3 {

enum class R3_API QueueType {
    Present,
    Graphics,
    Compute,
    Transfer,
};

struct R3_API ShaderStageFlags {
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

struct R3_API MemoryPropertyFlags {
    enum : uint32 {
        DeviceLocal       = 0x00000001,
        HostVisible       = 0x00000002,
        HostCoherent      = 0x00000004,
        HostCached        = 0x00000008,
        LazilyAllocated   = 0x00000010,
        Protected         = 0x00000020,
        DeviceCoherentAmd = 0x00000040,
        DeviceUncachedAmd = 0x00000080,
        RdmaCapableNv     = 0x00000100,
    };
};
using MemoryProperties = uint32;

struct R3_API BufferUsageFlags {
    enum : uint32 {
        TransferSrc        = 0x00000001,
        TransferDst        = 0x00000002,
        UniformTexelBuffer = 0x00000004,
        StorageTexelBuffer = 0x00000008,
        UniformBuffer      = 0x00000010,
        StorageBuffer      = 0x00000020,
        IndexBuffer        = 0x00000040,
        VertexBuffer       = 0x00000080,
        IndirectBuffer     = 0x00000100,
    };
};
using BufferUsage = uint32;

struct R3_API ImageUsageFlags {
    enum : uint32 {
        TransferSrc            = 0x00000001,
        TransferDst            = 0x00000002,
        Sampled                = 0x00000004,
        Storage                = 0x00000008,
        ColorAttachment        = 0x00000010,
        DepthStencilAttachment = 0x00000020,
        TransientAttachment    = 0x00000040,
        InputAttachment        = 0x00000080,
        HostTransfer           = 0x00400000,
    };
};
using ImageUsage = uint32;

enum class R3_API ImageTiling {
    Optimal = 0,
    Linear  = 1,
};

enum class R3_API TextureType {
    Albedo            = 0,
    MetallicRoughness = 1,
    Normal            = 2,
    AmbientOcclusion  = 3,
    Emissive          = 4,
    CubeMap           = 5,
};

enum class R3_API ShaderType {
    Vertex   = 0,
    Fragment = 1,
    Compute  = 2,
    // TessellationControl    = 3,
    // TessellationEvaluation = 4,
    // Geometry               = 5,
    // Raygen                 = 6,
    // AnyHit                 = 7,
    // ClosestHit             = 8,
    // Miss                   = 9,
    // Intersection           = 10,
    // Callable               = 11,
    // Task                   = 12,
    // Mesh                   = 13,
    // SubpassShading         = 14,
    // ClusterCulling         = 15,
};

enum class R3_API BufferType {
    Staging = 0,
    Vertex  = 1,
    Index   = 2,
    Uniform = 3,
    Storage = 4,
    // Indirect = 5,
};

enum class R3_API BufferPreset {
    Staging,
    HostUniform,
    HostStorage,
    DeviceStorage,
    DeviceVertex,
    DeviceIndex,
};

struct R3_API CommandPoolModeFlags {
    enum : uint32 {
        Transient = 0x00000001,
        Reset     = 0x00000002,
        Protected = 0x00000004,
    };
};
using CommandPoolMode = uint32;

} // namespace R3

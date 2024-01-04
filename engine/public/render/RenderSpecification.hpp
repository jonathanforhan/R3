#pragma once

/// @brief Copied from vulkan_core.h
/// All values are Vulkan Native, convert for capability with other renderers

#include "api/Flag.hpp"

namespace R3 {

static constexpr auto PBR_TEXTURE_COUNT = 5;    ///< Number of Textures used for PBR Renderering
static constexpr auto MAX_FRAMES_IN_FLIGHT = 3; ///< Maximum frames in queue at one time

/// @brief Flags for Image Usage
struct R3_API ImageUsage : public Flag {
    enum {
        TransferSrc = 0x01,
        TransferDst = 0x02,
        Sampled = 0x04,
        Storage = 0x08,
        ColorAttachment = 0x10,
        DepthStencilAttachment = 0x20,
        TransientAttachment = 0x40,
        InputAttachment = 0x80,
    };
};

/// @brief Flags for Image Aspects
struct R3_API ImageAspect : public Flag {
    enum {
        None = 0x00,
        Color = 0x01,
        Depth = 0x02,
        Stencil = 0x04,
    };
};

/// @brief Image Layouts
enum class R3_API ImageLayout {
    Undefined = 0,
    General = 1,
    ColorAttachmentOptimal = 2,
    DepthStencilAttachmentOptimal = 3,
    DepthStencilReadOnlyOptimal = 4,
    ShaderReadOnlyOptimal = 5,
    TransferSrcOptimal = 6,
    TransferDstOptimal = 7,
    Preinitialized = 8,
    DepthReadOnlyStencilAttachmentOptimal = 1000117000,
    DepthAttachmentStencilReadOnlyOptimal = 1000117001,
    DepthAttachmentOptimal = 1000241000,
    DepthReadOnlyOptimal = 1000241001,
    StencilAttachmentOptimal = 1000241002,
    StencilReadOnlyOptimal = 1000241003,
    ReadOnlyOptimal = 1000314000,
    AttachmentOptimal = 1000314001,
    PresentSrc = 1000001002,
};

/// @brief Types of Image copies supported
enum class R3_API ImageCopyType {
    Image,
    BufferToImage,
};

/// @brief Color Formats
enum class R3_API Format {
    Undefined = 0,
    R4G4UnormPack8 = 1,
    R4G4B4A4UnormPack16 = 2,
    B4G4R4A4UnormPack16 = 3,
    R5G6B5UnormPack16 = 4,
    B5G6R5UnormPack16 = 5,
    R5G5B5A1UnormPack16 = 6,
    B5G5R5A1UnormPack16 = 7,
    A1R5G5B5UnormPack16 = 8,
    R8Unorm = 9,
    R8Snorm = 10,
    R8Uscaled = 11,
    R8Sscaled = 12,
    R8Uint = 13,
    R8Sint = 14,
    R8Srgb = 15,
    R8G8Unorm = 16,
    R8G8Snorm = 17,
    R8G8Uscaled = 18,
    R8G8Sscaled = 19,
    R8G8Uint = 20,
    R8G8Sint = 21,
    R8G8Srgb = 22,
    R8G8B8Unorm = 23,
    R8G8B8Snorm = 24,
    R8G8B8Uscaled = 25,
    R8G8B8Sscaled = 26,
    R8G8B8Uint = 27,
    R8G8B8Sint = 28,
    R8G8B8Srgb = 29,
    B8G8R8Unorm = 30,
    B8G8R8Snorm = 31,
    B8G8R8Uscaled = 32,
    B8G8R8Sscaled = 33,
    B8G8R8Uint = 34,
    B8G8R8Sint = 35,
    B8G8R8Srgb = 36,
    R8G8B8A8Unorm = 37,
    R8G8B8A8Snorm = 38,
    R8G8B8A8Uscaled = 39,
    R8G8B8A8Sscaled = 40,
    R8G8B8A8Uint = 41,
    R8G8B8A8Sint = 42,
    R8G8B8A8Srgb = 43,
    B8G8R8A8Unorm = 44,
    B8G8R8A8Snorm = 45,
    B8G8R8A8Uscaled = 46,
    B8G8R8A8Sscaled = 47,
    B8G8R8A8Uint = 48,
    B8G8R8A8Sint = 49,
    B8G8R8A8Srgb = 50,
    A8B8G8R8UnormPack32 = 51,
    A8B8G8R8SnormPack32 = 52,
    A8B8G8R8UscaledPack32 = 53,
    A8B8G8R8SscaledPack32 = 54,
    A8B8G8R8UintPack32 = 55,
    A8B8G8R8SintPack32 = 56,
    A8B8G8R8SrgbPack32 = 57,
    A2R10G10B10UnormPack32 = 58,
    A2R10G10B10SnormPack32 = 59,
    A2R10G10B10UscaledPack32 = 60,
    A2R10G10B10SscaledPack32 = 61,
    A2R10G10B10UintPack32 = 62,
    A2R10G10B10SintPack32 = 63,
    A2B10G10R10UnormPack32 = 64,
    A2B10G10R10SnormPack32 = 65,
    A2B10G10R10UscaledPack32 = 66,
    A2B10G10R10SscaledPack32 = 67,
    A2B10G10R10UintPack32 = 68,
    A2B10G10R10SintPack32 = 69,
    R16Unorm = 70,
    R16Snorm = 71,
    R16Uscaled = 72,
    R16Sscaled = 73,
    R16Uint = 74,
    R16Sint = 75,
    R16Sfloat = 76,
    R16G16Unorm = 77,
    R16G16Snorm = 78,
    R16G16Uscaled = 79,
    R16G16Sscaled = 80,
    R16G16Uint = 81,
    R16G16Sint = 82,
    R16G16Sfloat = 83,
    R16G16B16Unorm = 84,
    R16G16B16Snorm = 85,
    R16G16B16Uscaled = 86,
    R16G16B16Sscaled = 87,
    R16G16B16Uint = 88,
    R16G16B16Sint = 89,
    R16G16B16Sfloat = 90,
    R16G16B16A16Unorm = 91,
    R16G16B16A16Snorm = 92,
    R16G16B16A16Uscaled = 93,
    R16G16B16A16Sscaled = 94,
    R16G16B16A16Uint = 95,
    R16G16B16A16Sint = 96,
    R16G16B16A16Sfloat = 97,
    R32Uint = 98,
    R32Sint = 99,
    R32Sfloat = 100,
    R32G32Uint = 101,
    R32G32Sint = 102,
    R32G32Sfloat = 103,
    R32G32B32Uint = 104,
    R32G32B32Sint = 105,
    R32G32B32Sfloat = 106,
    R32G32B32A32Uint = 107,
    R32G32B32A32Sint = 108,
    R32G32B32A32Sfloat = 109,
    R64Uint = 110,
    R64Sint = 111,
    R64Sfloat = 112,
    R64G64Uint = 113,
    R64G64Sint = 114,
    R64G64Sfloat = 115,
    R64G64B64Uint = 116,
    R64G64B64Sint = 117,
    R64G64B64Sfloat = 118,
    R64G64B64A64Uint = 119,
    R64G64B64A64Sint = 120,
    R64G64B64A64Sfloat = 121,
    B10G11R11UfloatPack32 = 122,
    E5B9G9R9UfloatPack32 = 123,
    D16Unorm = 124,
    X8D24UnormPack32 = 125,
    D32Sfloat = 126,
    S8Uint = 127,
};

/// @brief Presentation Modes
enum class R3_API PresentMode {
    Immediate = 0,
    Mailbox = 1,
    Fifo = 2,
    FifoRelaxed = 3,
};

/// @brief ColorSpaces
enum class R3_API ColorSpace {
    SrgbNonlinear = 0,
    DisplayP3NonlinearEXT = 1000104001,
    ExtendedSrgbLinearEXT = 1000104002,
    DisplayP3LinearEXT = 1000104003,
    DciP3NonlinearEXT = 1000104004,
    Bt709LinearEXT = 1000104005,
    Bt709NonlinearEXT = 1000104006,
    Bt2020LinearEXT = 1000104007,
    Hdr10St2084EXT = 1000104008,
    DolbyvisionEXT = 1000104009,
    Hdr10HlgEXT = 1000104010,
    AdobergbLinearEXT = 1000104011,
    AdobergbNonlinearEXT = 1000104012,
    PassThroughEXT = 1000104013,
    ExtendedSrgbNonlinearEXT = 1000104014,
    DisplayNativeAMD = 1000213000,
};

struct R3_API MemoryAccessor : public Flag {
    enum {
        IndirectCommandRead = 0x0000'0001,
        IndexRead = 0x0000'0002,
        VertexAttributeRead = 0x0000'0004,
        UniformRead = 0x0000'0008,
        InputAttachmentRead = 0x0000'0010,
        ShaderRead = 0x0000'0020,
        ShaderWrite = 0x0000'0040,
        ColorAttachmentRead = 0x0000'0080,
        ColorAttachmentWrite = 0x0000'0100,
        DepthStencilAttachmentRead = 0x0000'0200,
        DepthStencilAttachmentWrite = 0x0000'0400,
        TransferRead = 0x0000'0800,
        TransferWrite = 0x0000'1000,
        HostRead = 0x0000'2000,
        HostWrite = 0x0000'4000,
        MemoryRead = 0x0000'8000,
        MemoryWrite = 0x0001'0000,
        None = 0x0000'0000,
    };
};

/// @brief PipelineStage
struct R3_API PipelineStage : public Flag {
    enum {
        TopOfPipe = 0x0000'0001,
        DrawIndirect = 0x0000'0002,
        VertexInput = 0x0000'0004,
        VertexShader = 0x0000'0008,
        TessellationControlShader = 0x0000'0010,
        TessellationEvaluationShader = 0x0000'0020,
        GeometryShader = 0x0000'0040,
        FragmentShader = 0x0000'0080,
        EarlyFragmentTests = 0x0000'0100,
        LateFragmentTests = 0x0000'0200,
        ColorAttachmentOutput = 0x0000'0400,
        ComputeShader = 0x0000'0800,
        Transfer = 0x0000'1000,
        BottomOfPipe = 0x0000'2000,
        Host = 0x0000'4000,
        AllGraphics = 0x0000'8000,
        AllCommands = 0x0001'0000,
        None = 0x0000'0000,
        TransformFeedback = 0x0100'0000,
        ConditionalRendering = 0x0004'0000,
        AccelerationStructureBuild = 0x0200'0000,
        RayTracingShader = 0x0020'0000,
        ShadingRateImageNV = 0x0000'0000,
        RayTracingShaderNV = 0x0000'0000,
        AccelerationStructureBuildNV = 0x0000'0000,
        TaskShaderNV = 0x0008'0000,
        MeshShaderNV = 0x0010'0000,
        FragmentDensityProcess = 0x0080'0000,
        FragmentShadingRateAttachment = 0x0040'0000,
        CommandPreprocessNV = 0x0002'0000,
    };
};

/// @brief BufferUsage
struct R3_API BufferUsage : public Flag {
    enum {
        TransferSrc = 0x0000'0001,
        TransferDst = 0x0000'0002,
        UniformTexelBuffer = 0x0000'0004,
        StorageTexelBuffer = 0x0000'0008,
        UniformBuffer = 0x0000'0010,
        StorageBuffer = 0x0000'0020,
        IndexBuffer = 0x0000'0040,
        VertexBuffer = 0x0000'0080,
        IndirectBuffer = 0x0000'0100,
        ShaderDeviceAddress = 0x0002'0000,
        VideoDecodeSrc = 0x0000'2000,
        VideoDecodeDst = 0x0000'4000,
        TransformFeedbackBuffer = 0x0000'0800,
        TransformFeedbackCounterBuffer = 0x0000'1000,
        ConditionalRendering = 0x0000'0200,
        SamplerDescriptorBuffer = 0x0020'0000,
        ResourceDescriptorBuffer = 0x0040'0000,
        PushDescriptorsDescriptorBuffer = 0x0400'0000,
        MicromapBuildInputReadOnly = 0x0080'0000,
        MicromapStorage = 0x0100'0000,
    };
};

/// @brief MemoryProperty
struct R3_API MemoryProperty : public Flag {
    enum {
        DeviceLocal = 0x0000'0001,
        HostVisible = 0x0000'0002,
        HostCoherent = 0x0000'0004,
        HostCached = 0x0000'0008,
        LazilyAllocated = 0x0000'0010,
        Protected = 0x0000'0020,
        DeviceCoherentAMD = 0x0000'0040,
        DeviceUncachedAMD = 0x0000'0080,
        RdmaCapableNV = 0x0000'0100,
    };
};

/// @brief DescriptorType
enum class R3_API DescriptorType {
    Sampler = 0,
    CombinedImageSampler = 1,
    SampledImage = 2,
    StorageImage = 3,
    UniformTexelBuffer = 4,
    StorageTexelBuffer = 5,
    UniformBuffer = 6,
    StorageBuffer = 7,
    UniformBufferDynamic = 8,
    StorageBufferDynamic = 9,
    InputAttachment = 10,
    InlineUniformBlock = 1000138000,
};

struct R3_API ShaderStage : public Flag {
    enum {
        Vertex = 0x00000001,
        TessellationControl = 0x00000002,
        TessellationEvaluation = 0x00000004,
        Geometry = 0x00000008,
        Fragment = 0x00000010,
        Compute = 0x00000020,
        AllGraphics = 0x0000001F,
        All = 0x7FFFFFFF,
        Raygen = 0x00000100,
        AnyHit = 0x00000200,
        ClosestHit = 0x00000400,
        Miss = 0x00000800,
        Intersection = 0x00001000,
        Callable = 0x00002000,
        RaygenNV = 0x00000040,
        AnyHitNV = 0x00000080,
        ClosestHitNV = 0x00004000,
        MissNV = 0x00080000,
    };
};

enum class R3_API VertexInputRate {
    Vertex = 0,
    Instance = 1,
};

} // namespace R3
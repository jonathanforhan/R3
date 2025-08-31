/// @file glTF.hpp

#pragma once

#include <array>
#include <concepts>
#include <map>
#include <memory>
#include <optional>
#include <string>
#include <type_traits>
#include <vector>
#include "engine/api/JSON.hpp"
#include "engine/api/Types.hpp"

/// glTF file format spec info
/// https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html
/// public version info is stored in engine/public/api/Version.hpp
///
/// glTF version major : 2
/// glTF version minor : 0
/// glb container version : 2

/*
    x - supported
    w - work-in-progress

    === Supported Extensions ===

    --- Ratified ---
[ ] KHR_draco_mesh_compression
[ ] KHR_lights_punctual
[ ] KHR_materials_anisotropy
[ ] KHR_materials_clearcoat
[ ] KHR_materials_emissive_strength
[ ] KHR_materials_ior
[ ] KHR_materials_iridescence
[ ] KHR_materials_sheen
[ ] KHR_materials_specular
[ ] KHR_materials_transmission
[ ] KHR_materials_unlit
[ ] KHR_materials_variants
[ ] KHR_materials_volume
[ ] KHR_mesh_quantization
[ ] KHR_texture_basisu
[ ] KHR_texture_transform
[ ] KHR_xmp_json_ld
[ ] EXT_mesh_gpu_instancing
[ ] EXT_meshopt_compression
[ ] EXT_texture_webp

    --- Multi-Vendor ---
[ ] EXT_lights_ies
[ ] EXT_lights_image_based
[ ] EXT_mesh_manifold

    --- Vendor ---
[ ] ADOBE_materials_clearcoat_specular
[ ] ADOBE_materials_clearcoat_tint
[ ] ADOBE_materials_thin_transparency
[ ] AGI_articulations
[ ] AGI_stk_metadata
[ ] CESIUM_primitive_outline
[ ] FB_geometry_metadata
[ ] GRIFFEL_bim_data
[ ] MPEG_accessor_timed
[ ] MPEG_animation_timing
[ ] MPEG_audio_spatial
[ ] MPEG_buffer_circular
[ ] MPEG_media
[ ] MPEG_mesh_linking
[ ] MPEG_scene_dynamic
[ ] MPEG_texture_video
[ ] MPEG_viewport_recommended
[ ] MSFT_lod
[ ] MSFT_packing_normalRoughnessMetallic
[ ] MSFT_packing_occlusionRoughnessMetallic
[ ] MSFT_texture_dds
[ ] NV_materials_mdl

    --- Archived ---
[ ] KHR_materials_pbrSpecularGlossiness
[ ] KHR_techniques_webgl
[ ] KHR_xmp

    --- In-Progress ---
[ ] KHR_animation_pointer
[ ] KHR_audio
[ ] KHR_materials_diffuse_transmission
[ ] KHR_materials_dispersion
[ ] KHR_materials_sss
*/

namespace R3::glTF {

static constexpr auto HEADER_MAGIC = 0x46546C67;

static constexpr auto CHUNK_TYPE_JSON = 0x4E4F534A;
static constexpr auto CHUNK_TYPE_BIN  = 0x004E4942;

static constexpr auto BYTE           = 5120;
static constexpr auto UNSIGNED_BYTE  = 5121;
static constexpr auto SHORT          = 5122;
static constexpr auto UNSIGNED_SHORT = 5123;
static constexpr auto INT            = 5124;
static constexpr auto UNSIGNED_INT   = 5125;
static constexpr auto FLOAT          = 5126;

static constexpr auto SCALAR = "SCALAR";
static constexpr auto VEC2   = "VEC2";
static constexpr auto VEC3   = "VEC3";
static constexpr auto VEC4   = "VEC4";
static constexpr auto MAT2   = "MAT2";
static constexpr auto MAT3   = "MAT3";
static constexpr auto MAT4   = "MAT4";

static constexpr auto POINTS         = 0;
static constexpr auto LINES          = 1;
static constexpr auto LINE_LOOP      = 2;
static constexpr auto LINE_STRIP     = 3;
static constexpr auto TRIANGLES      = 4;
static constexpr auto TRIANGLE_STRIP = 5;
static constexpr auto TRIANGLE_FAN   = 6;

static constexpr auto NEAREST                = 9728;
static constexpr auto LINEAR                 = 9729;
static constexpr auto NEAREST_MIPMAP_NEAREST = 9984;
static constexpr auto LINEAR_MIPMAP_NEAREST  = 9985;
static constexpr auto NEAREST_MIPMAP_LINEAR  = 9986;
static constexpr auto LINEAR_MIPMAP_LINEAR   = 9987;

static constexpr auto CLAMP_TO_EDGE   = 33071;
static constexpr auto MIRRORED_REPEAT = 33648;
static constexpr auto REPEAT          = 10497;

static constexpr auto ARRAY_BUFFER         = 34962;
static constexpr auto ELEMENT_ARRAY_BUFFER = 34963;

static constexpr auto POSITION   = "POSITION";
static constexpr auto NORMAL     = "NORMAL";
static constexpr auto TANGENT    = "TANGENT";
static constexpr auto TEXCOORD_0 = "TEXCOORD_0";
static constexpr auto TEXCOORD_1 = "TEXCOORD_1";
static constexpr auto TEXCOORD_2 = "TEXCOORD_2";
static constexpr auto TEXCOORD_3 = "TEXCOORD_3";
static constexpr auto COLOR_0    = "COLOR_0";
static constexpr auto COLOR_1    = "COLOR_1";
static constexpr auto COLOR_2    = "COLOR_2";
static constexpr auto COLOR_3    = "COLOR_3";
static constexpr auto JOINTS_0   = "JOINTS_0";
static constexpr auto JOINTS_1   = "JOINTS_1";
static constexpr auto JOINTS_2   = "JOINTS_2";
static constexpr auto JOINTS_3   = "JOINTS_3";
static constexpr auto WEIGHTS_0  = "WEIGHTS_0";
static constexpr auto WEIGHTS_1  = "WEIGHTS_1";
static constexpr auto WEIGHTS_2  = "WEIGHTS_2";
static constexpr auto WEIGHTS_3  = "WEIGHTS_3";

static constexpr auto OPAQUE = "OPAQUE";
static constexpr auto MASK   = "MASK";
static constexpr auto BLEND  = "BLEND";

static constexpr auto SAMPLER_LINEAR      = "LINEAR";
static constexpr auto SAMPLER_STEP        = "STEP";
static constexpr auto SAMPLER_CUBICSPLINE = "CUBICSPLINE";

/// @brief Parameter required by glTF spec
/// @tparam T Datatype
template <typename T>
using Required = T;

/// @brief Parameter not required by glTF spec but has default value
/// @tparam T Datatype
template <typename T>
using Default = T;

namespace detail {

template <typename T>
concept HasEmptyState = requires(T t) { t.empty(); } || std::same_as<T, json::Value>;

template <typename T>
using OptionalType = std::conditional_t<HasEmptyState<T>, T, std::optional<T>>;

} // namespace detail

/// @brief Parameter not required by glTF spec
/// @tparam T Datatype
template <typename T>
using Optional = typename detail::OptionalType<T>;

/// https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#binary-header
struct Header {
    alignas(4) uint32 magic;
    alignas(4) uint32 version;
    alignas(4) uint32 length;
};

/// https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#chunks
struct ChunkHeader {
    alignas(4) uint32 length;
    alignas(4) uint32 type;
};

/// Virtual Base class for Supported glTF extensions
struct Extension {
    Extension(const char* name)
        : name(name) {}

    virtual ~Extension() {}

    const char* name;
};

struct Accessor;
struct AccessorSparse;
struct AccessorSparseIndices;
struct AccessorSparseValues;
struct Animation;
struct AnimationChannel;
struct AnimationChannelTarget;
struct AnimationSampler;
struct Asset;
struct Buffer;
struct BufferView;
struct Camera;
struct CameraOrthographic;
struct CameraPerspective;
struct Root; // aka glTF
struct Image;
struct Material;
struct NormalTextureInfo;
struct OcclusionTextureInfo;
struct PBRMetallicRoughness;
struct Mesh;
struct MeshPrimitive;
struct Node;
struct Sampler;
struct Scene;
struct Skin;
struct Texture;
struct TextureInfo;

/// https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#reference-accessor-sparse-indices
struct AccessorSparseIndices {
    Required<uint32> bufferView;
    Default<uint32> byteOffset = 0;
    Required<uint32> componentType;
    Optional<json::Value> extensions;
    Optional<json::Value> extras;
};

/// https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#reference-accessor-sparse-values
struct AccessorSparseValues {
    Required<uint32> bufferView;
    Default<uint32> byteOffset = 0;
    Optional<json::Value> extensions;
    Optional<json::Value> extras;
};

/// https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#reference-accessor-sparse
struct AccessorSparse {
    Required<uint32> count;
    Required<AccessorSparseIndices> indices;
    Required<AccessorSparseValues> values;
    Optional<json::Value> extensions;
    Optional<json::Value> extras;
};

/// https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#reference-accessor
struct Accessor {
    Optional<uint32> bufferView;
    Default<uint32> byteOffset = 0;
    Required<uint32> componentType;
    Default<bool> normalized = false;
    Required<uint32> count;
    Required<std::string> type;
    Optional<std::vector<float>> max;
    Optional<std::vector<float>> min;
    Optional<AccessorSparse> sparse;
    Optional<std::string> name;
    Optional<json::Value> extensions;
    Optional<json::Value> extras;
};

/// https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#reference-animation-channel-target
struct AnimationChannelTarget {
    Optional<uint32> node;
    Required<std::string> path;
    Optional<json::Value> extensions;
    Optional<json::Value> extras;
};

/// https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#reference-animation-channel
struct AnimationChannel {
    Required<uint32> sampler;
    Required<AnimationChannelTarget> target;
    Optional<json::Value> extensions;
    Optional<json::Value> extras;
};

/// https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#reference-animation-sampler
struct AnimationSampler {
    Required<uint32> input;
    Default<std::string> interpolation = SAMPLER_LINEAR;
    Required<uint32> output;
    Optional<json::Value> extensions;
    Optional<json::Value> extras;
};

/// https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#reference-animation
struct Animation {
    Required<std::vector<AnimationChannel>> channels;
    Required<std::vector<AnimationSampler>> samplers;
    Optional<std::string> name;
    Optional<json::Value> extensions;
    Optional<json::Value> extras;
};

/// https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#reference-asset
struct Asset {
    Optional<std::string> copyright;
    Optional<std::string> generator;
    Required<std::string> version;
    Optional<std::string> minVersion;
    Optional<json::Value> extensions;
    Optional<json::Value> extras;
};

/// https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#reference-buffer
struct Buffer {
    Optional<std::string> uri;
    Required<uint32> byteLength;
    Optional<std::string> name;
    Optional<json::Value> extensions;
    Optional<json::Value> extras;
};

/// https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#reference-bufferview
struct BufferView {
    Required<uint32> buffer;
    Default<uint32> byteOffset = 0;
    Required<uint32> byteLength;
    Optional<uint32> byteStride;
    Optional<uint32> target;
    Optional<std::string> name;
    Optional<json::Value> extensions;
    Optional<json::Value> extras;
};

/// https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#reference-camera-orthographic
struct CameraOrthographic {
    Required<float> xmag;
    Required<float> ymag;
    Required<float> zfar;
    Required<float> znear;
    Optional<json::Value> extensions;
    Optional<json::Value> extras;
};

/// https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#reference-camera-perspective
struct CameraPerspective {
    Optional<float> aspectRatio;
    Required<float> yfov;
    Optional<float> zfar;
    Required<float> znear;
    Optional<json::Value> extensions;
    Optional<json::Value> extras;
};

/// https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#reference-camera
struct Camera {
    Optional<CameraOrthographic> orthographic;
    Optional<CameraPerspective> perspective;
    Required<std::string> type;
    Optional<std::string> name;
    Optional<json::Value> extensions;
    Optional<json::Value> extras;
};

/// https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#reference-extension
/// DOCUMENTATION

/// https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#reference-extras
/// DOCUMENTATION

/// https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#reference-image
struct Image {
    Optional<std::string> uri;
    Optional<std::string> mimeType;
    Optional<uint32> bufferView;
    Optional<std::string> name;
    Optional<json::Value> extensions;
    Optional<json::Value> extras;
};

/// https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#reference-texture
struct Texture {
    Optional<uint32> sampler;
    Optional<uint32> source;
    Optional<std::string> name;
    Optional<json::Value> extensions;
    Optional<json::Value> extras;
};

/// https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#reference-textureinfo
struct TextureInfo {
    Required<uint32> index;
    Default<uint32> texCoord = 0;
    Optional<json::Value> extensions;
    Optional<json::Value> extras;
};

/// https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#reference-material-normaltextureinfo
struct NormalTextureInfo {
    Required<uint32> index;
    Default<uint32> texCoord = 0;
    Default<float> scale     = 1.0f;
    Optional<json::Value> extensions;
    Optional<json::Value> extras;
};

/// https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#reference-material-occlusiontextureinfo
struct OcclusionTextureInfo {
    Required<uint32> index;
    Default<uint32> texCoord = 0;
    Default<float> strength  = 1.0f;
    Optional<json::Value> extensions;
    Optional<json::Value> extras;
};

/// https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#reference-material-pbrmetallicroughness
struct PBRMetallicRoughness {
    Default<std::array<float, 4>> baseColorFactor = std::array{1.0f, 1.0f, 1.0f, 1.0f};
    Optional<TextureInfo> baseColorTexture;
    Default<float> metallicFactor  = 1.0f;
    Default<float> roughnessFactor = 1.0f;
    Optional<TextureInfo> metallicRoughnessTexture;
    Optional<json::Value> extensions;
    Optional<json::Value> extras;
};

/// https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#reference-material
struct Material {
    Optional<PBRMetallicRoughness> pbrMetallicRoughness;
    Optional<NormalTextureInfo> normalTexture;
    Optional<OcclusionTextureInfo> occlusionTexture;
    Optional<TextureInfo> emissiveTexture;
    Default<std::array<float, 3>> emissiveFactor = std::array{0.0f, 0.0f, 0.0f};
    Default<std::string> alphaMode               = OPAQUE;
    Default<float> alphaCutoff                   = 0.5f;
    Default<bool> doubleSided                    = false;
    Optional<std::string> name;
    Optional<std::vector<std::unique_ptr<glTF::Extension>>> extensions;
    Optional<json::Value> extras;
};

/// https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#reference-mesh-primitive
struct MeshPrimitive {
    Required<std::map<std::string, uint32>> attributes;
    Optional<uint32> indices;
    Optional<uint32> material;
    Default<uint32> mode = TRIANGLES;
    Optional<std::vector<std::map<std::string, uint32>>> targets;
    Optional<json::Value> extensions;
    Optional<json::Value> extras;
};

/// https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#reference-mesh
struct Mesh {
    Required<std::vector<MeshPrimitive>> primitives;
    Optional<std::vector<float>> weights;
    Optional<std::string> name;
    Optional<json::Value> extensions;
    Optional<json::Value> extras;
};

/// https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#reference-node
struct Node {
    Optional<uint32> camera;
    Optional<std::vector<uint32>> children;
    Optional<uint32> skin;
    Default<std::array<float, 16>> matrix =
        std::array{1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f};
    Optional<uint32> mesh;
    Default<std::array<float, 4>> rotation    = std::array{0.0f, 0.0f, 0.0f, 1.0f};
    Default<std::array<float, 3>> scale       = std::array{1.0f, 1.0f, 1.0f};
    Default<std::array<float, 3>> translation = std::array{0.0f, 0.0f, 0.0f};
    Optional<std::vector<float>> weights;
    Optional<std::string> name;
    Optional<json::Value> extensions;
    Optional<json::Value> extras;
};

/// https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#reference-sampler
struct Sampler {
    Optional<uint32> magFilter;
    Optional<uint32> minFilter;
    Default<uint32> wrapS = REPEAT;
    Default<uint32> wrapT = REPEAT;
    Optional<std::string> name;
    Optional<json::Value> extensions;
    Optional<json::Value> extras;
};

/// https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#reference-scene
struct Scene {
    Optional<std::vector<uint32>> nodes;
    Optional<std::string> name;
    Optional<json::Value> extensions;
    Optional<json::Value> extras;
};

/// https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#reference-skin
struct Skin {
    Optional<uint32> inverseBindMatrices;
    Optional<uint32> skeleton;
    Required<std::vector<uint32>> joints;
    Optional<std::string> name;
    Optional<json::Value> extensions;
    Optional<json::Value> extras;
};

/// https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#reference-gltf
struct Root {
    Optional<std::vector<std::string>> extensionsUsed;
    Optional<std::vector<std::string>> extensionsRequired;
    Optional<std::vector<Accessor>> accessors;
    Optional<std::vector<Animation>> animations;
    Required<Asset> asset;
    Optional<std::vector<Buffer>> buffers;
    Optional<std::vector<BufferView>> bufferViews;
    Optional<std::vector<Camera>> cameras;
    Optional<std::vector<Image>> images;
    Optional<std::vector<Material>> materials;
    Optional<std::vector<Mesh>> meshes;
    Optional<std::vector<Node>> nodes;
    Optional<std::vector<Sampler>> samplers;
    Optional<uint32> scene;
    Optional<std::vector<Scene>> scenes;
    Optional<std::vector<Skin>> skins;
    Optional<std::vector<Texture>> textures;
    Optional<json::Value> extensions;
    Optional<json::Value> extras;
};

} // namespace R3::glTF

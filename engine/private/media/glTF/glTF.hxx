#pragma once
#pragma warning(push)
#pragma warning(disable : 26495)

#include <rapidjson/document.h>
#include <optional>
#include "api/Types.hpp"

#if R3_GLTF_JSON_EXTRAS
#define GLTF_EXTRAS std::optional<rapidjson::Value> extras = std::nullopt
#else
#define GLTF_EXTRAS
#endif

// glTF file format spec info
// https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html
// public version info is stored in engine/public/api/Version.hpp
//
// glTF version major : 2
// glTF version minor : 0
// glb container version : 2

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
[w] KHR_materials_pbrSpecularGlossiness (currently only supports diffuse color)
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
static constexpr auto CHUNK_TYPE_BIN = 0x004E4942;

static constexpr auto BYTE = 5120;
static constexpr auto UNSIGNED_BYTE = 5121;
static constexpr auto SHORT = 5122;
static constexpr auto UNSIGNED_SHORT = 5123;
static constexpr auto INT = 5124;
static constexpr auto UNSIGNED_INT = 5125;
static constexpr auto FLOAT = 5126;

static constexpr auto SCALAR = "SCALAR";
static constexpr auto VEC2 = "VEC2";
static constexpr auto VEC3 = "VEC3";
static constexpr auto VEC4 = "VEC4";
static constexpr auto MAT2 = "MAT2";
static constexpr auto MAT3 = "MAT3";
static constexpr auto MAT4 = "MAT4";

static constexpr auto POINTS = 0;
static constexpr auto LINES = 1;
static constexpr auto LINE_LOOP = 2;
static constexpr auto LINE_STRIP = 3;
static constexpr auto TRIANGLES = 4;
static constexpr auto TRIANGLE_STRIP = 5;
static constexpr auto TRIANGLE_FAN = 6;

static constexpr auto NEAREST = 9728;
static constexpr auto LINEAR = 9729;
static constexpr auto NEAREST_MIPMAP_NEAREST = 9984;
static constexpr auto LINEAR_MIPMAP_NEAREST = 9985;
static constexpr auto NEAREST_MIPMAP_LINEAR = 9986;
static constexpr auto LINEAR_MIPMAP_LINEAR = 9987;

static constexpr auto CLAMP_TO_EDGE = 33071;
static constexpr auto MIRRORED_REPEAT = 33648;
static constexpr auto REPEAT = 10497;

static constexpr auto ARRAY_BUFFER = 34962;
static constexpr auto ELEMENT_ARRAY_BUFFER = 34963;

static constexpr auto POSITION = "POSITION";
static constexpr auto NORMAL = "NORMAL";
static constexpr auto TANGENT = "TANGENT";
static constexpr auto TEXCOORD_0 = "TEXCOORD_0";
static constexpr auto TEXCOORD_1 = "TEXCOORD_1";
static constexpr auto TEXCOORD_2 = "TEXCOORD_2";
static constexpr auto TEXCOORD_3 = "TEXCOORD_3";
static constexpr auto COLOR_0 = "COLOR_0";
static constexpr auto COLOR_1 = "COLOR_1";
static constexpr auto COLOR_2 = "COLOR_2";
static constexpr auto COLOR_3 = "COLOR_3";
static constexpr auto JOINTS_0 = "JOINTS_0";
static constexpr auto JOINTS_1 = "JOINTS_1";
static constexpr auto JOINTS_2 = "JOINTS_2";
static constexpr auto JOINTS_3 = "JOINTS_3";
static constexpr auto WEIGHTS_0 = "WEIGHTS_0";
static constexpr auto WEIGHTS_1 = "WEIGHTS_1";
static constexpr auto WEIGHTS_2 = "WEIGHTS_2";
static constexpr auto WEIGHTS_3 = "WEIGHTS_3";

static constexpr auto OPAQUE = "OPAQUE";
static constexpr auto MASK = "MASK";
static constexpr auto BLEND = "BLEND";

static constexpr auto SAMPLER_LINEAR = "LINEAR";
static constexpr auto SAMPLER_STEP = "STEP";
static constexpr auto SAMPLER_CUBICSPLINE = "CUBICSPLINE";

// https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#binary-header
struct Header {
    alignas(4) uint32 magic;
    alignas(4) uint32 version;
    alignas(4) uint32 length;
};

// https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#chunks
struct ChunkHeader {
    alignas(4) uint32 length;
    alignas(4) uint32 type;
};

// Virtual Base class for Supported glTF extensions
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

// https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#reference-accessor-sparse-indices
struct AccessorSparseIndices {
    uint32 bufferView; // REQUIRED
    uint32 byteOffset = 0;
    uint32 componentType; // REQUIRED
    std::optional<rapidjson::Value> extensions;
    GLTF_EXTRAS;
};

// https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#reference-accessor-sparse-values
struct AccessorSparseValues {
    uint32 bufferView; // REQUIRED
    uint32 byteOffset = 0;
    std::optional<rapidjson::Value> extensions;
    GLTF_EXTRAS;
};

// https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#reference-accessor-sparse
struct AccessorSparse {
    uint32 count;                  // REQUIRED
    AccessorSparseIndices indices; // REQUIRED
    AccessorSparseValues values;   // REQUIRED
    std::optional<rapidjson::Value> extensions;
    GLTF_EXTRAS;
};

// https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#reference-accessor
struct Accessor {
    uint32 bufferView = undefined;
    uint32 byteOffset = 0;
    uint32 componentType; // REQUIRED
    bool normalized = false;
    uint32 count;     // REQUIRED
    std::string type; // REQUIRED
    std::vector<float> max;
    std::vector<float> min;
    AccessorSparse sparse = {};
    std::string name;
    std::optional<rapidjson::Value> extensions;
    GLTF_EXTRAS;
};

// https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#reference-animation-channel-target
struct AnimationChannelTarget {
    uint32 node = undefined;
    std::string path; // REQUIRED
    std::optional<rapidjson::Value> extensions;
    GLTF_EXTRAS;
};

// https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#reference-animation-channel
struct AnimationChannel {
    uint32 sampler;                // REQUIRED
    AnimationChannelTarget target; // REQUIRED
    std::optional<rapidjson::Value> extensions;
    GLTF_EXTRAS;
};

// https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#reference-animation-sampler
struct AnimationSampler {
    uint32 input; // REQUIRED
    std::string interpolation = SAMPLER_LINEAR;
    uint32 output; // REQUIRED
    std::optional<rapidjson::Value> extensions;
    GLTF_EXTRAS;
};

// https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#reference-animation
struct Animation {
    std::vector<AnimationChannel> channels; // REQUIRED
    std::vector<AnimationSampler> samplers; // REQUIRED
    std::string name;
    std::optional<rapidjson::Value> extensions;
    GLTF_EXTRAS;
};

// https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#reference-asset
struct Asset {
    std::string copyright;
    std::string generator;
    std::string version; // REQUIRED
    std::string minVersion;
    std::optional<rapidjson::Value> extensions;
    GLTF_EXTRAS;
};

// https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#reference-buffer
struct Buffer {
    std::string uri;
    uint32 byteLength; // REQUIRED
    std::string name;
    std::optional<rapidjson::Value> extensions;
    GLTF_EXTRAS;
};

// https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#reference-bufferview
struct BufferView {
    uint32 buffer; // REQUIRED
    uint32 byteOffset = 0;
    uint32 byteLength; // REQUIRED
    uint32 byteStride = undefined;
    uint32 target = undefined;
    std::string name;
    std::optional<rapidjson::Value> extensions;
    GLTF_EXTRAS;
};

// https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#reference-camera-orthographic
struct CameraOrthographic {
    float xmag;  // REQUIRED
    float ymag;  // REQUIRED
    float zfar;  // REQUIRED
    float znear; // REQUIRED
    std::optional<rapidjson::Value> extensions;
    GLTF_EXTRAS;
};

// https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#reference-camera-perspective
struct CameraPerspective {
    float aspectRation = 0;
    float yfov; // REQUIRED
    float zfar = 0;
    float znear; // REQUIRED
    std::optional<rapidjson::Value> extensions;
    GLTF_EXTRAS;
};

// https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#reference-camera
struct Camera {
    CameraOrthographic orthographic = {};
    CameraPerspective perspective = {};
    std::string type; // REQUIRED
    std::string name;
    std::optional<rapidjson::Value> extensions;
    GLTF_EXTRAS;
};

// https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#reference-extension
// DOCUMENTATION

// https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#reference-extras
// DOCUMENTATION

// https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#reference-image
struct Image {
    std::string uri;
    std::string mimeType;
    uint32 bufferView = undefined;
    std::string name;
    std::optional<rapidjson::Value> extensions;
    GLTF_EXTRAS;
};

// https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#reference-texture
struct Texture {
    uint32 sampler = undefined;
    uint32 source = undefined;
    std::string name;
    std::optional<rapidjson::Value> extensions;
    GLTF_EXTRAS;
};

// https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#reference-textureinfo
struct TextureInfo {
    uint32 index; // REQUIRED
    uint32 texCoord = 0;
    std::optional<rapidjson::Value> extensions;
    GLTF_EXTRAS;
};

// https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#reference-material-normaltextureinfo
struct NormalTextureInfo {
    uint32 index; // REQUIRED
    uint32 texCoord = 0;
    float scale = 1.0f;
    std::optional<rapidjson::Value> extensions;
    GLTF_EXTRAS;
};

// https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#reference-material-occlusiontextureinfo
struct OcclusionTextureInfo {
    uint32 index; // REQUIRED
    uint32 texCoord = 0;
    float strength = 1.0f;
    std::optional<rapidjson::Value> extensions;
    GLTF_EXTRAS;
};

// https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#reference-material-pbrmetallicroughness
struct PBRMetallicRoughness {
    float baseColorFactor[4] = {1.0f, 1.0f, 1.0f, 1.0f};
    std::optional<TextureInfo> baseColorTexture;
    float metallicFactor = 1.0f;
    float roughnessFactor = 1.0f;
    std::optional<TextureInfo> metallicRoughnessTexture;
    std::optional<rapidjson::Value> extensions;
    GLTF_EXTRAS;
};

// https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#reference-material
struct Material {
    PBRMetallicRoughness pbrMetallicRoughness = {};
    std::optional<NormalTextureInfo> normalTexture;
    std::optional<OcclusionTextureInfo> occlusionTexture;
    std::optional<TextureInfo> emissiveTexture;
    float emissiveFactor[3] = {0.0f, 0.0f, 0.0f};
    std::string alphaMode = OPAQUE;
    float alphaCutoff = 0.5;
    bool doubleSided = false;
    std::string name;
    std::vector<std::unique_ptr<glTF::Extension>> extensions;
    GLTF_EXTRAS;
};

// https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#reference-mesh-primitive
struct MeshPrimitive {
    rapidjson::Value attributes; // REQUIRED
    uint32 indices = undefined;
    uint32 material = undefined;
    uint32 mode{4};
    std::vector<rapidjson::Value> targets;
    std::optional<rapidjson::Value> extensions;
    GLTF_EXTRAS;
};

// https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#reference-mesh
struct Mesh {
    std::vector<MeshPrimitive> primitives; // REQUIRED
    std::vector<float> weights;
    std::string name;
    std::optional<rapidjson::Value> extensions;
    GLTF_EXTRAS;
};

// https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#reference-node
struct Node {
    uint32 camera = undefined;
    std::vector<uint32> children;
    uint32 skin = undefined;
    float matrix[16] = {1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f};
    uint32 mesh = undefined;
    float rotation[4] = {0.0f, 0.0f, 0.0f, 1.0f};
    float scale[3] = {1.0f, 1.0f, 1.0f};
    float translation[3] = {0.0f, 0.0f, 0.0f};
    std::vector<float> weights;
    std::string name;
    std::optional<rapidjson::Value> extensions;
    GLTF_EXTRAS;
};

// https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#reference-sampler
struct Sampler {
    uint32 magFilter = undefined;
    uint32 minFilter = undefined;
    uint32 wrapS{REPEAT};
    uint32 wrapT{REPEAT};
    std::string name;
    std::optional<rapidjson::Value> extensions;
    GLTF_EXTRAS;
};

// https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#reference-scene
struct Scene {
    std::vector<uint32> nodes;
    std::string name;
    std::optional<rapidjson::Value> extensions;
    GLTF_EXTRAS;
};

// https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#reference-skin
struct Skin {
    uint32 inverseBindMatrices = undefined;
    uint32 skeleton = undefined;
    std::vector<uint32> joints; // REQUIRED
    std::string name;
    std::optional<rapidjson::Value> extensions;
    GLTF_EXTRAS;
};

// https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#reference-gltf
struct Root {
    std::vector<std::string> extensionsUsed;
    std::vector<std::string> extensionsRequired;
    std::vector<Accessor> accessors;
    std::vector<Animation> animations;
    Asset asset; // REQUIRED
    std::vector<Buffer> buffers;
    std::vector<BufferView> bufferViews;
    std::vector<Camera> cameras;
    std::vector<Image> images;
    std::vector<Material> materials;
    std::vector<Mesh> meshes;
    std::vector<Node> nodes;
    std::vector<Sampler> samplers;
    uint32 scene = undefined;
    std::vector<Scene> scenes;
    std::vector<Skin> skins;
    std::vector<Texture> textures;
    std::optional<rapidjson::Value> extensions;
    GLTF_EXTRAS;
};

} // namespace R3::glTF

#pragma warning(pop)

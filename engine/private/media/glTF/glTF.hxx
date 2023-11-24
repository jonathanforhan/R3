#pragma once
#include <rapidjson/document.h>
#include <optional>
#include "api/Types.hpp"

// glTF file format spec info
// https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html
// public version info is stored in engine/public/api/Version.hpp
//
// glTF version major : 2
// glTF version minor : 0
// glb container version : 2

// TODO support ratified extensions

/*
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
*/

namespace R3::glTF {

constexpr auto HEADER_MAGIC = 0x46546C67;

constexpr auto CHUNK_TYPE_JSON = 0x4E4F534A;
constexpr auto CHUNK_TYPE_BIN = 0x004E4942;

constexpr auto BYTE = 5120;
constexpr auto UNSIGNED_BYTE = 5121;
constexpr auto SHORT = 5122;
constexpr auto UNSIGNED_SHORT = 5123;
constexpr auto INT = 5124;
constexpr auto UNSIGNED_INT = 5125;
constexpr auto FLOAT = 5126;

constexpr auto SCALAR = "SCALAR";
constexpr auto VEC2 = "VEC2";
constexpr auto VEC3 = "VEC3";
constexpr auto VEC4 = "VEC4";
constexpr auto MAT2 = "MAT2";
constexpr auto MAT3 = "MAT3";
constexpr auto MAT4 = "MAT4";

constexpr auto POINTS = 0;
constexpr auto LINES = 1;
constexpr auto LINE_LOOP = 2;
constexpr auto LINE_STRIP = 3;
constexpr auto TRIANGLES = 4;
constexpr auto TRIANGLE_STRIP = 5;
constexpr auto TRIANGLE_FAN = 6;

constexpr auto NEAREST = 9728;
constexpr auto LINEAR = 9729;
constexpr auto NEAREST_MIPMAP_NEAREST = 9984;
constexpr auto LINEAR_MIPMAP_NEAREST = 9985;
constexpr auto NEAREST_MIPMAP_LINEAR = 9986;
constexpr auto LINEAR_MIPMAP_LINEAR = 9987;

constexpr auto CLAMP_TO_EDGE = 33071;
constexpr auto MIRRORED_REPEAT = 33648;
constexpr auto REPEAT = 10497;

constexpr auto ARRAY_BUFFER = 34962;
constexpr auto ELEMENT_ARRAY_BUFFER = 34963;

constexpr auto POSITION = "POSITION";
constexpr auto NORMAL = "NORMAL";
constexpr auto TANGENT = "TANGENT";
constexpr auto TEXCOORD_0 = "TEXCOORD_0";
constexpr auto TEXCOORD_1 = "TEXCOORD_1";
constexpr auto TEXCOORD_2 = "TEXCOORD_2";
constexpr auto TEXCOORD_3 = "TEXCOORD_3";
constexpr auto COLOR_0 = "COLOR_0";
constexpr auto COLOR_1 = "COLOR_1";
constexpr auto COLOR_2 = "COLOR_2";
constexpr auto COLOR_3 = "COLOR_3";
constexpr auto JOINTS_0 = "JOINTS_0";
constexpr auto JOINTS_1 = "JOINTS_1";
constexpr auto JOINTS_2 = "JOINTS_2";
constexpr auto JOINTS_3 = "JOINTS_3";
constexpr auto WEIGHTS_0 = "WEIGHTS_0";
constexpr auto WEIGHTS_1 = "WEIGHTS_1";
constexpr auto WEIGHTS_2 = "WEIGHTS_2";
constexpr auto WEIGHTS_3 = "WEIGHTS_3";

constexpr auto OPAQUE = "OPAQUE";
constexpr auto MASK = "MASK";
constexpr auto BLEND = "BLEND";

constexpr auto SAMPLER_LINEAR = "LINEAR";
constexpr auto SAMPLER_STEP = "STEP";
constexpr auto SAMPLER_CUBICSPLINE = "CUBICSPLINE";

constexpr auto UNDEFINED = UINT32_MAX;

#pragma pack(push, 1)

// https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#binary-header
struct Header {
    uint32 magic;
    uint32 version;
    uint32 length;
};

// https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#chunks
struct ChunkHeader {
    uint32 length;
    uint32 type;
};

#pragma pack(pop)

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
    uint32 byteOffset{0};
    uint32 componentType; // REQUIRED
    std::optional<rapidjson::Value> extensions{std::nullopt};
    std::optional<rapidjson::Value> extras{std::nullopt};
};

// https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#reference-accessor-sparse-values
struct AccessorSparseValues {
    uint32 bufferView; // REQUIRED
    uint32 byteOffset{0};
    std::optional<rapidjson::Value> extensions{std::nullopt};
    std::optional<rapidjson::Value> extras{std::nullopt};
};

// https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#reference-accessor-sparse
struct AccessorSparse {
    uint32 count;                  // REQUIRED
    AccessorSparseIndices indices; // REQUIRED
    AccessorSparseValues values;   // REQUIRED
    std::optional<rapidjson::Value> extensions{std::nullopt};
    std::optional<rapidjson::Value> extras{std::nullopt};
};

// https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#reference-accessor
struct Accessor {
    uint32 bufferView{UNDEFINED};
    uint32 byteOffset{0};
    uint32 componentType; // REQUIRED
    bool normalized{false};
    uint32 count;     // REQUIRED
    std::string type; // REQUIRED
    std::vector<float> max;
    std::vector<float> min;
    AccessorSparse sparse{};
    std::string name;
    std::optional<rapidjson::Value> extensions{std::nullopt};
    std::optional<rapidjson::Value> extras{std::nullopt};
};

// https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#reference-animation-channel-target
struct AnimationChannelTarget {
    uint32 node{UNDEFINED};
    std::string path; // REQUIRED
    std::optional<rapidjson::Value> extensions{std::nullopt};
    std::optional<rapidjson::Value> extras{std::nullopt};
};

// https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#reference-animation-channel
struct AnimationChannel {
    uint32 sampler;                // REQUIRED
    AnimationChannelTarget target; // REQUIRED
    std::optional<rapidjson::Value> extensions{std::nullopt};
    std::optional<rapidjson::Value> extras{std::nullopt};
};

// https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#reference-animation-sampler
struct AnimationSampler {
    uint32 input; // REQUIRED
    std::string interpolation{SAMPLER_LINEAR};
    uint32 output; // REQUIRED
    std::optional<rapidjson::Value> extensions{std::nullopt};
    std::optional<rapidjson::Value> extras{std::nullopt};
};

// https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#reference-animation
struct Animation {
    std::vector<AnimationChannel> channels; // REQUIRED
    std::vector<AnimationSampler> samplers; // REQUIRED
    std::string name;
    std::optional<rapidjson::Value> extensions;
    std::optional<rapidjson::Value> extras;
};

// https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#reference-asset
struct Asset {
    std::string copyright;
    std::string generator;
    std::string version; // REQUIRED
    std::string minVersion;
    std::optional<rapidjson::Value> extensions{std::nullopt};
    std::optional<rapidjson::Value> extras{std::nullopt};
};

// https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#reference-buffer
struct Buffer {
    std::string uri;
    uint32 byteLength; // REQUIRED
    std::string name;
    std::optional<rapidjson::Value> extensions;
    std::optional<rapidjson::Value> extras;
};

// https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#reference-bufferview
struct BufferView {
    uint32 buffer; // REQUIRED
    uint32 byteOffset{0};
    uint32 byteLength; // REQUIRED
    uint32 byteStride{UNDEFINED};
    uint32 target{UNDEFINED};
    std::string name;
    std::optional<rapidjson::Value> extensions;
    std::optional<rapidjson::Value> extras;
};

// https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#reference-camera-orthographic
struct CameraOrthographic {
    float xmag;  // REQUIRED
    float ymag;  // REQUIRED
    float zfar;  // REQUIRED
    float znear; // REQUIRED
    std::optional<rapidjson::Value> extensions;
    std::optional<rapidjson::Value> extras;
};

// https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#reference-camera-perspective
struct CameraPerspective {
    float aspectRation{0};
    float yfov; // REQUIRED
    float zfar{0};
    float znear; // REQUIRED
    std::optional<rapidjson::Value> extensions;
    std::optional<rapidjson::Value> extras;
};

// https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#reference-camera
struct Camera {
    CameraOrthographic orthographic{};
    CameraPerspective perspective{};
    std::string type; // REQUIRED
    std::string name;
    std::optional<rapidjson::Value> extensions;
    std::optional<rapidjson::Value> extras;
};

// https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#reference-extension
// DOCUMENTATION

// https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#reference-extras
// DOCUMENTATION

// https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#reference-image
struct Image {
    std::string uri;
    std::string mimeType;
    uint32 bufferView{UNDEFINED};
    std::string name;
    std::optional<rapidjson::Value> extensions;
    std::optional<rapidjson::Value> extras;
};

// https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#reference-texture
struct Texture {
    uint32 sampler{UNDEFINED};
    uint32 source{UNDEFINED};
    std::string name;
    std::optional<rapidjson::Value> extensions{std::nullopt};
    std::optional<rapidjson::Value> extras{std::nullopt};
};

// https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#reference-textureinfo
struct TextureInfo {
    uint32 index; // REQUIRED
    uint32 texCoord{0};
    std::optional<rapidjson::Value> extensions{std::nullopt};
    std::optional<rapidjson::Value> extras{std::nullopt};
};

// https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#reference-material-normaltextureinfo
struct NormalTextureInfo {
    uint32 index; // REQUIRED
    uint32 texCoord{0};
    float scale{1.0f};
    std::optional<rapidjson::Value> extensions;
    std::optional<rapidjson::Value> extras;
};

// https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#reference-material-occlusiontextureinfo
struct OcclusionTextureInfo {
    uint32 index; // REQUIRED
    uint32 texCoord{0};
    float strength{1.0f};
    std::optional<rapidjson::Value> extensions;
    std::optional<rapidjson::Value> extras;
};

// https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#reference-material-pbrmetallicroughness
struct PBRMetallicRoughness {
    float baseColorFactor[4]{1.0f, 1.0f, 1.0f, 1.0f};
    std::optional<TextureInfo> baseColorTexture{std::nullopt};
    float metallicFactor{1.0f};
    float roughnessFactor{1.0f};
    std::optional<TextureInfo> metallicRoughnessTexture{std::nullopt};
    std::optional<rapidjson::Value> extensions;
    std::optional<rapidjson::Value> extras;
};

// https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#reference-material
struct Material {
    std::optional<PBRMetallicRoughness> pbrMetallicRoughness{std::nullopt};
    std::optional<NormalTextureInfo> normalTexture{std::nullopt};
    std::optional<OcclusionTextureInfo> occlusionTexture{std::nullopt};
    std::optional<TextureInfo> emissiveTexture{std::nullopt};
    float emissiveFactor[3]{0.0f, 0.0f, 0.0f};
    std::string alphaMode{OPAQUE};
    float alphaCutoff{0.5};
    bool doubleSided{false};
    std::string name;
    std::optional<rapidjson::Value> extensions;
    std::optional<rapidjson::Value> extras;
};

// https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#reference-mesh-primitive
struct MeshPrimitive {
    rapidjson::Value attributes; // REQUIRED
    uint32 indices{UNDEFINED};
    uint32 material{UNDEFINED};
    uint32 mode{4};
    std::vector<rapidjson::Value> targets;
    std::optional<rapidjson::Value> extensions;
    std::optional<rapidjson::Value> extras;
};

// https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#reference-mesh
struct Mesh {
    std::vector<MeshPrimitive> primitives; // REQUIRED
    std::vector<float> weights;
    std::string name;
    std::optional<rapidjson::Value> extensions;
    std::optional<rapidjson::Value> extras;
};

// https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#reference-node
struct Node {
    uint32 camera{UNDEFINED};
    std::vector<uint32> children;
    uint32 skin{UNDEFINED};
    float matrix[16] = {1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f};
    uint32 mesh{UNDEFINED};
    float rotation[4] = {0.0f, 0.0f, 0.0f, 1.0f};
    float scale[3] = {1.0f, 1.0f, 1.0f};
    float translation[3] = {0.0f, 0.0f, 0.0f};
    std::vector<float> weights;
    std::string name;
    std::optional<rapidjson::Value> extensions;
    std::optional<rapidjson::Value> extras;
};

// https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#reference-sampler
struct Sampler {
    uint32 magFilter{UNDEFINED};
    uint32 minFilter{UNDEFINED};
    uint32 wrapS{REPEAT};
    uint32 wrapT{REPEAT};
    std::string name;
    std::optional<rapidjson::Value> extensions;
    std::optional<rapidjson::Value> extras;
};

// https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#reference-scene
struct Scene {
    std::vector<uint32> nodes;
    std::string name;
    std::optional<rapidjson::Value> extensions;
    std::optional<rapidjson::Value> extras;
};

// https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#reference-skin
struct Skin {
    uint32 inverseBindMatrices{UNDEFINED};
    uint32 skeleton{UNDEFINED};
    std::vector<uint32> joints; // REQUIRED
    std::string name;
    std::optional<rapidjson::Value> extensions;
    std::optional<rapidjson::Value> extras;
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
    uint32 scene{UNDEFINED};
    std::vector<Scene> scenes;
    std::vector<Skin> skins;
    std::vector<Texture> textures;
    std::optional<rapidjson::Value> extensions;
    std::optional<rapidjson::Value> extras;
};

} // namespace R3::glTF

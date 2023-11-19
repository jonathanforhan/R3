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

namespace R3 {

constexpr auto GLTF_HEADER_MAGIC = 0x46546C67;

constexpr auto GLTF_CHUNK_TYPE_JSON = 0x4E4F534A;
constexpr auto GLTF_CHUNK_TYPE_BIN = 0x004E4942;

constexpr auto GLTF_BYTE = 5120;
constexpr auto GLTF_UNSIGNED_BYTE = 5121;
constexpr auto GLTF_SHORT = 5122;
constexpr auto GLTF_UNSIGNED_SHORT = 5123;
constexpr auto GLTF_INT = 5124;
constexpr auto GLTF_UNSIGNED_INT = 5125;
constexpr auto GLTF_FLOAT = 5126;

constexpr auto GLTF_SCALAR = "SCALAR";
constexpr auto GLTF_VEC2 = "VEC2";
constexpr auto GLTF_VEC3 = "VEC3";
constexpr auto GLTF_VEC4 = "VEC4";
constexpr auto GLTF_MAT2 = "MAT2";
constexpr auto GLTF_MAT3 = "MAT3";
constexpr auto GLTF_MAT4 = "MAT4";

constexpr auto GLTF_POINTS = 0;
constexpr auto GLTF_LINES = 1;
constexpr auto GLTF_LINE_LOOP = 2;
constexpr auto GLTF_LINE_STRIP = 3;
constexpr auto GLTF_TRIANGLES = 4;
constexpr auto GLTF_TRIANGLE_STRIP = 5;
constexpr auto GLTF_TRIANGLE_FAN = 6;

constexpr auto GLTF_NEAREST = 9728;
constexpr auto GLTF_LINEAR = 9729;
constexpr auto GLTF_NEAREST_MIPMAP_NEAREST = 9984;
constexpr auto GLTF_LINEAR_MIPMAP_NEAREST = 9985;
constexpr auto GLTF_NEAREST_MIPMAP_LINEAR = 9986;
constexpr auto GLTF_LINEAR_MIPMAP_LINEAR = 9987;

constexpr auto GLTF_CLAMP_TO_EDGE = 33071;
constexpr auto GLTF_MIRRORED_REPEAT = 33648;
constexpr auto GLTF_REPEAT = 10497;

constexpr auto GLTF_ARRAY_BUFFER = 34962;
constexpr auto GLTF_ELEMENT_ARRAY_BUFFER = 34963;

constexpr auto GLTF_POSITION = "POSITION";
constexpr auto GLTF_NORMAL = "NORMAL";
constexpr auto GLTF_TANGENT = "TANGENT";
constexpr auto GLTF_TEXCOORD_0 = "TEXCOORD_0";
constexpr auto GLTF_TEXCOORD_1 = "TEXCOORD_1";
constexpr auto GLTF_TEXCOORD_2 = "TEXCOORD_2";
constexpr auto GLTF_TEXCOORD_3 = "TEXCOORD_3";
constexpr auto GLTF_COLOR_0 = "COLOR_0";
constexpr auto GLTF_COLOR_1 = "COLOR_1";
constexpr auto GLTF_COLOR_2 = "COLOR_2";
constexpr auto GLTF_COLOR_3 = "COLOR_3";
constexpr auto GLTF_JOINTS_0 = "JOINTS_0";
constexpr auto GLTF_JOINTS_1 = "JOINTS_1";
constexpr auto GLTF_JOINTS_2 = "JOINTS_2";
constexpr auto GLTF_JOINTS_3 = "JOINTS_3";
constexpr auto GLTF_WEIGHTS_0 = "WEIGHTS_0";
constexpr auto GLTF_WEIGHTS_1 = "WEIGHTS_1";
constexpr auto GLTF_WEIGHTS_2 = "WEIGHTS_2";
constexpr auto GLTF_WEIGHTS_3 = "WEIGHTS_3";

constexpr auto GLTF_OPAQUE = "OPAQUE";
constexpr auto GLTF_MASK = "MASK";
constexpr auto GLTF_BLEND = "BLEND";

constexpr auto GLTF_SAMPLER_LINEAR = "LINEAR";
constexpr auto GLTF_SAMPLER_STEP = "STEP";
constexpr auto GLTF_SAMPLER_CUBICSPLINE = "CUBICSPLINE";

constexpr auto GLTF_UNDEFINED = UINT32_MAX;

#pragma pack(push, 1)

// https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#binary-header
struct GLTF_Header {
    uint32 magic;
    uint32 version;
    uint32 length;
};

// https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#chunks
struct GLTF_ChunkHeader {
    uint32 length;
    uint32 type;
};

#pragma pack(pop)

struct GLTF_Accessor;
struct GLTF_AccessorSparse;
struct GLTF_AccessorSparseIndices;
struct GLTF_AccessorSparseValues;
struct GLTF_Animation;
struct GLTF_AnimationChannel;
struct GLTF_AnimationChannelTarget;
struct GLTF_AnimationSampler;
struct GLTF_Asset;
struct GLTF_Buffer;
struct GLTF_BufferView;
struct GLTF_Camera;
struct GLTF_CameraOrthographic;
struct GLTF_CameraPerspective;
struct GLTF_Root; // aka glTF
struct GLTF_Image;
struct GLTF_Material;
struct GLTF_NormalTextureInfo;
struct GLTF_OcclusionTextureInfo;
struct GLTF_PBRMetallicRoughness;
struct GLTF_Mesh;
struct GLTF_MeshPrimitive;
struct GLTF_Node;
struct GLTF_Sampler;
struct GLTF_Scene;
struct GLTF_Skin;
struct GLTF_Texture;
struct GLTF_TextureInfo;

// https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#reference-accessor-sparse-indices
struct GLTF_AccessorSparseIndices {
    uint32 bufferView; // REQUIRED
    uint32 byteOffset{0};
    uint32 componentType; // REQUIRED
    std::optional<rapidjson::Value> extensions{std::nullopt};
    std::optional<rapidjson::Value> extras{std::nullopt};
};

// https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#reference-accessor-sparse-values
struct GLTF_AccessorSparseValues {
    uint32 bufferView; // REQUIRED
    uint32 byteOffset{0};
    std::optional<rapidjson::Value> extensions{std::nullopt};
    std::optional<rapidjson::Value> extras{std::nullopt};
};

// https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#reference-accessor-sparse
struct GLTF_AccessorSparse {
    uint32 count;                       // REQUIRED
    GLTF_AccessorSparseIndices indices; // REQUIRED
    GLTF_AccessorSparseValues values;   // REQUIRED
    std::optional<rapidjson::Value> extensions{std::nullopt};
    std::optional<rapidjson::Value> extras{std::nullopt};
};

// https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#reference-accessor
struct GLTF_Accessor {
    uint32 bufferView{GLTF_UNDEFINED};
    uint32 byteOffset{0};
    uint32 componentType; // REQUIRED
    bool normalized{false};
    uint32 count;     // REQUIRED
    std::string type; // REQUIRED
    std::vector<float> max;
    std::vector<float> min;
    GLTF_AccessorSparse sparse{};
    std::string name;
    std::optional<rapidjson::Value> extensions{std::nullopt};
    std::optional<rapidjson::Value> extras{std::nullopt};
};

// https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#reference-animation-channel-target
struct GLTF_AnimationChannelTarget {
    uint32 node{GLTF_UNDEFINED};
    std::string path; // REQUIRED
    std::optional<rapidjson::Value> extensions{std::nullopt};
    std::optional<rapidjson::Value> extras{std::nullopt};
};

// https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#reference-animation-channel
struct GLTF_AnimationChannel {
    uint32 sampler;                     // REQUIRED
    GLTF_AnimationChannelTarget target; // REQUIRED
    std::optional<rapidjson::Value> extensions{std::nullopt};
    std::optional<rapidjson::Value> extras{std::nullopt};
};

// https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#reference-animation-sampler
struct GLTF_AnimationSampler {
    uint32 input; // REQUIRED
    std::string interpolation{GLTF_SAMPLER_LINEAR};
    uint32 output; // REQUIRED
    std::optional<rapidjson::Value> extensions{std::nullopt};
    std::optional<rapidjson::Value> extras{std::nullopt};
};

// https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#reference-animation
struct GLTF_Animation {
    std::vector<GLTF_AnimationChannel> channels; // REQUIRED
    std::vector<GLTF_AnimationSampler> samplers; // REQUIRED
    std::string name;
    std::optional<rapidjson::Value> extensions;
    std::optional<rapidjson::Value> extras;
};

// https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#reference-asset
struct GLTF_Asset {
    std::string copyright;
    std::string generator;
    std::string version; // REQUIRED
    std::string minVersion;
    std::optional<rapidjson::Value> extensions{std::nullopt};
    std::optional<rapidjson::Value> extras{std::nullopt};
};

// https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#reference-buffer
struct GLTF_Buffer {
    std::string uri;
    uint32 byteLength; // REQUIRED
    std::string name;
    std::optional<rapidjson::Value> extensions;
    std::optional<rapidjson::Value> extras;
};

// https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#reference-bufferview
struct GLTF_BufferView {
    uint32 buffer; // REQUIRED
    uint32 byteOffset{0};
    uint32 byteLength; // REQUIRED
    uint32 byteStride{GLTF_UNDEFINED};
    uint32 target{GLTF_UNDEFINED};
    std::string name;
    std::optional<rapidjson::Value> extensions;
    std::optional<rapidjson::Value> extras;
};

// https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#reference-camera-orthographic
struct GLTF_CameraOrthographic {
    float xmag;  // REQUIRED
    float ymag;  // REQUIRED
    float zfar;  // REQUIRED
    float znear; // REQUIRED
    std::optional<rapidjson::Value> extensions;
    std::optional<rapidjson::Value> extras;
};

// https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#reference-camera-perspective
struct GLTF_CameraPerspective {
    float aspectRation{0};
    float yfov; // REQUIRED
    float zfar{0};
    float znear; // REQUIRED
    std::optional<rapidjson::Value> extensions;
    std::optional<rapidjson::Value> extras;
};

// https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#reference-camera
struct GLTF_Camera {
    GLTF_CameraOrthographic orthographic{};
    GLTF_CameraPerspective perspective{};
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
struct GLTF_Image {
    std::string uri;
    std::string mimeType;
    uint32 bufferView{GLTF_UNDEFINED};
    std::string name;
    std::optional<rapidjson::Value> extensions;
    std::optional<rapidjson::Value> extras;
};

// https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#reference-texture
struct GLTF_Texture {
    uint32 sampler{GLTF_UNDEFINED};
    uint32 source{GLTF_UNDEFINED};
    std::string name;
    std::optional<rapidjson::Value> extensions{std::nullopt};
    std::optional<rapidjson::Value> extras{std::nullopt};
};

// https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#reference-textureinfo
struct GLTF_TextureInfo {
    uint32 index; // REQUIRED
    uint32 texCoord{0};
    std::optional<rapidjson::Value> extensions{std::nullopt};
    std::optional<rapidjson::Value> extras{std::nullopt};
};

// https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#reference-material-normaltextureinfo
struct GLTF_NormalTextureInfo {
    uint32 index; // REQUIRED
    uint32 texCoord{0};
    float scale{1.0f};
    std::optional<rapidjson::Value> extensions;
    std::optional<rapidjson::Value> extras;
};

// https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#reference-material-occlusiontextureinfo
struct GLTF_OcclusionTextureInfo {
    uint32 index; // REQUIRED
    uint32 texCoord{0};
    float strength{1.0f};
    std::optional<rapidjson::Value> extensions;
    std::optional<rapidjson::Value> extras;
};

// https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#reference-material-pbrmetallicroughness
struct GLTF_PBRMetallicRoughness {
    float baseColorFactor[4]{1.0f, 1.0f, 1.0f, 1.0f};
    std::optional<GLTF_TextureInfo> baseColorTexture{std::nullopt};
    float metallicFactor{1.0f};
    float roughnessFactor{1.0f};
    std::optional<GLTF_TextureInfo> metallicRoughnessTexture{std::nullopt};
    std::optional<rapidjson::Value> extensions;
    std::optional<rapidjson::Value> extras;
};

// https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#reference-material
struct GLTF_Material {
    std::optional<GLTF_PBRMetallicRoughness> pbrMetallicRoughness{std::nullopt};
    std::optional<GLTF_NormalTextureInfo> normalTexture{std::nullopt};
    std::optional<GLTF_OcclusionTextureInfo> occlusionTexture{std::nullopt};
    std::optional<GLTF_TextureInfo> emissiveTexture{std::nullopt};
    float emissiveFactor[3]{0.0f, 0.0f, 0.0f};
    std::string alphaMode{GLTF_OPAQUE};
    float alphaCutoff{0.5};
    bool doubleSided{false};
    std::string name;
    std::optional<rapidjson::Value> extensions;
    std::optional<rapidjson::Value> extras;
};

// https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#reference-mesh-primitive
struct GLTF_MeshPrimitive {
    rapidjson::Value attributes; // REQUIRED
    uint32 indices{GLTF_UNDEFINED};
    uint32 material{GLTF_UNDEFINED};
    uint32 mode{4};
    std::vector<rapidjson::Value> targets;
    std::optional<rapidjson::Value> extensions;
    std::optional<rapidjson::Value> extras;
};

// https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#reference-mesh
struct GLTF_Mesh {
    std::vector<GLTF_MeshPrimitive> primitives; // REQUIRED
    std::vector<float> weights;
    std::string name;
    std::optional<rapidjson::Value> extensions;
    std::optional<rapidjson::Value> extras;
};

// https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#reference-node
struct GLTF_Node {
    uint32 camera{GLTF_UNDEFINED};
    std::vector<uint32> children;
    uint32 skin{GLTF_UNDEFINED};
    float matrix[16] = {1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f};
    uint32 mesh{GLTF_UNDEFINED};
    float rotation[4] = {0.0f, 0.0f, 0.0f, 1.0f};
    float scale[3] = {1.0f, 1.0f, 1.0f};
    float translation[3] = {0.0f, 0.0f, 0.0f};
    std::vector<float> weights;
    std::string name;
    std::optional<rapidjson::Value> extensions;
    std::optional<rapidjson::Value> extras;
};

// https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#reference-sampler
struct GLTF_Sampler {
    uint32 magFilter{GLTF_UNDEFINED};
    uint32 minFilter{GLTF_UNDEFINED};
    uint32 wrapS{GLTF_REPEAT};
    uint32 wrapT{GLTF_REPEAT};
    std::string name;
    std::optional<rapidjson::Value> extensions;
    std::optional<rapidjson::Value> extras;
};

// https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#reference-scene
struct GLTF_Scene {
    std::vector<uint32> nodes;
    std::string name;
    std::optional<rapidjson::Value> extensions;
    std::optional<rapidjson::Value> extras;
};

// https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#reference-skin
struct GLTF_Skin {
    uint32 inverseBindMatrices{GLTF_UNDEFINED};
    uint32 skeleton{GLTF_UNDEFINED};
    std::vector<uint32> joints; // REQUIRED
    std::string name;
    std::optional<rapidjson::Value> extensions;
    std::optional<rapidjson::Value> extras;
};

// https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#reference-gltf
struct GLTF_Root {
    std::vector<std::string> extensionsUsed;
    std::vector<std::string> extensionsRequired;
    std::vector<GLTF_Accessor> accessors;
    std::vector<GLTF_Animation> animations;
    GLTF_Asset asset; // REQUIRED
    std::vector<GLTF_Buffer> buffers;
    std::vector<GLTF_BufferView> bufferViews;
    std::vector<GLTF_Camera> cameras;
    std::vector<GLTF_Image> images;
    std::vector<GLTF_Material> materials;
    std::vector<GLTF_Mesh> meshes;
    std::vector<GLTF_Node> nodes;
    std::vector<GLTF_Sampler> samplers;
    uint32 scene{GLTF_UNDEFINED};
    std::vector<GLTF_Scene> scenes;
    std::vector<GLTF_Skin> skins;
    std::vector<GLTF_Texture> textures;
    std::optional<rapidjson::Value> extensions;
    std::optional<rapidjson::Value> extras;
};

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

} // namespace R3

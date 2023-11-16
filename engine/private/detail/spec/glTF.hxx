#pragma once
#include <rapidjson/fwd.h>
#include "api/Types.hpp"

// glTF file format spec info
// https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html
// public version info is stored in engine/public/api/Version.hpp
//
// glTF version major : 2
// glTF version minor : 0
// glb container version : 2

#ifdef R3_INCLUDE_GLTF_EXTENSIONS
#define GLTF_EXTENSIONS rapidjson::Value extensions
#else
#define GLTF_EXTENSIONS
#endif
#ifdef R3_INCLUDE_GLTF_EXTRAS
#define GLTF_EXTRAS rapidjson::Value extras
#else
#define GLTF_EXTRAS
#endif

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
// struct GLTF_AccessorSparse;
// struct GLTF_AccessorSparseIndices;
// struct GLTF_AccessorSparseValues;
// struct GLTF_Animation;
// struct GLTF_AnimationChannel;
// struct GLTF_AnimationChannelTarget;
// struct GLTF_AnimationSampler;
struct GLTF_Asset;
struct GLTF_Buffer;
struct GLTF_BufferView;
// struct GLTF_Camera;
// struct GLTF_CameraOrthographic;
// struct GLTF_CameraPerspective;
struct GLTF_Image;
// struct GLTF_Material;
// struct GLTF_MaterialNormalTextureInfo;
// struct GLTF_MaterialOcclusionTextureInfo;
// struct GLTF_MaterialPBRMetallicRoughness;
struct GLTF_Mesh;
struct GLTF_MeshPrimitive;
struct GLTF_Node;
struct GLTF_Sampler;
struct GLTF_Scene;
struct GLTF_Skin;
struct GLTF_Texture;
struct GLTF_TextureInfo;

// https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#reference-accessor
struct GLTF_Accessor {
    uint32 bufferView{GLTF_UNDEFINED};
    uint32 byteOffset{0};
    uint32 componentType; // REQUIRED
    bool normalized{false};
    uint32 count;     // REQUIRED
    std::string type; // REQUIRED
    uint32 max{GLTF_UNDEFINED};
    uint32 min{GLTF_UNDEFINED};
    // GLTF_AccessorSparse sparse | NOT IMPLEMENTED
    std::string name;
    GLTF_EXTENSIONS;
    GLTF_EXTRAS;
};

// https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#reference-accessor-sparse
// NOT IMPLEMENTED

// https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#reference-accessor-sparse-indices
// NOT IMPLEMENTED

// https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#reference-accessor-sparse-values
// NOT IMPLEMENTED

// https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#reference-animation
// NOT IMPLEMENTED

// https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#reference-animation-channel
// NOT IMPLEMENTED

// https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#reference-animation-channel-target
// NOT IMPLEMENTED

// https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#reference-animation-sampler
// NOT IMPLEMENTED

// https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#reference-asset
struct GLTF_Asset {
    std::string copyright;
    std::string generator;
    std::string version; // REQUIRED
    std::string minVersion;
    GLTF_EXTENSIONS;
    GLTF_EXTRAS;
};

// https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#reference-buffer
struct GLTF_Buffer {
    std::string uri;
    uint32 byteLength; // REQUIRED
    std::string name;
    GLTF_EXTENSIONS;
    GLTF_EXTRAS;
};

// https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#reference-bufferview
struct GLTF_BufferView {
    uint32 buffer; // REQUIRED
    uint32 byteOffset{0};
    uint32 byteLength; // REQUIRED
    uint32 byteStride{GLTF_UNDEFINED};
    uint32 target{GLTF_UNDEFINED};
    std::string name;
    GLTF_EXTENSIONS;
    GLTF_EXTRAS;
};

// https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#reference-camera
// NOT IMPLEMENTED

// https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#reference-camera-orthographic
// NOT IMPLEMENTED

// https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#reference-camera-perspective
// NOT IMPLEMENTED

// https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#reference-extension
// DOCUMENTATION

// https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#reference-extension
// DOCUMENTATION

// https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#reference-extension
// DOCUMENTATION : glTF is kept as a json object

// https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#reference-image
struct GLTF_Image {
    std::string uri;
    std::string mimeType;
    uint32 bufferView{GLTF_UNDEFINED};
    std::string name;
    GLTF_EXTENSIONS;
    GLTF_EXTRAS;
};

// https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#reference-material
// NOT IMPLEMENTED

// https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#reference-material-normaltextureinfo
// NOT IMPLEMENTED

// https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#reference-material-occlusiontextureinfo
// NOT IMPLEMENTED

// https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#reference-material-pbrmetallicroughness
// NOT IMPLEMENTED

// https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#reference-mesh
struct GLTF_Mesh {
    std::vector<GLTF_MeshPrimitive> primitives; // REQUIRED
    std::vector<float> weights;
    std::string name;
    GLTF_EXTENSIONS;
    GLTF_EXTRAS;
};

// https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#reference-mesh-primitive
struct GLTF_MeshPrimitive {
    rapidjson::Value attributes; // REQUIRED
    uint32 indices{GLTF_UNDEFINED};
    uint32 material{GLTF_UNDEFINED};
    uint32 mode{4};
    std::vector<uint32> targets;
    GLTF_EXTENSIONS;
    GLTF_EXTRAS;
};

// https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#reference-node
struct GLTF_Node {
    // uint32 camera | NOT IMPLEMENTED
    std::vector<uint32> children;
    uint32 skin{GLTF_UNDEFINED};
    float matrix[16] = {1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f};
    uint32 mesh{GLTF_UNDEFINED};
    float rotation[4] = {0.0f, 0.0f, 0.0f, 1.0f};
    float scale[3] = {1.0f, 1.0f, 1.0f};
    float translation[3] = {0.0f, 0.0f, 0.0f};
    std::vector<float> weights;
    std::string name;
    GLTF_EXTENSIONS;
    GLTF_EXTRAS;
};

// https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#reference-sampler
struct GLTF_Sampler {
    uint32 magFilter{GLTF_UNDEFINED};
    uint32 minFilter{GLTF_UNDEFINED};
    uint32 wrapS{GLTF_REPEAT};
    uint32 wrapT{GLTF_REPEAT};
    std::string name;
    GLTF_EXTENSIONS;
    GLTF_EXTRAS;
};

// https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#reference-scene
struct GLTF_Scene {
    std::vector<uint32> nodes;
    std::string name;
    GLTF_EXTENSIONS;
    GLTF_EXTRAS;
};

// https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#reference-skin
struct GLTF_Skin {
    uint32 inverseBindMatrices{GLTF_UNDEFINED};
    uint32 skeleton{GLTF_UNDEFINED};
    std::vector<uint32> joints; // REQUIRED
    std::string name;
    GLTF_EXTENSIONS;
    GLTF_EXTRAS;
};

// https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#reference-texture
struct GLTF_Texture {
    uint32 sample{GLTF_UNDEFINED};
    uint32 source{GLTF_UNDEFINED};
    std::string name;
    GLTF_EXTENSIONS;
    GLTF_EXTRAS;
};

// https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#reference-textureinfo
struct GLTF_TextureInfo {
    uint32 index; // REQUIRED
    uint32 texCoord{0};
    GLTF_EXTENSIONS;
    GLTF_EXTRAS;
};

} // namespace R3

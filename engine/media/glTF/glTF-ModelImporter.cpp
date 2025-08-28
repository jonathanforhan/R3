#include "glTF-ModelImporter.hpp"

#include <array>
#include <cstddef>
#include <cstdlib>
#include <filesystem>
#include <format>
#include <fstream>
#include <ios>
#include <istream>
#include <optional>
#include <ostream>
#include <sstream>
#include <string>
#include <string_view>
#include <type_traits>
#include <vector>
#include "api/Assert.hpp"
#include "api/Exception.hpp"
#include "api/JSON.hpp"
#include "api/Types.hpp"
#include "api/Version.hpp"
#include "core/Log.hpp"
#include "glTF.hpp"

namespace R3::glTF {

glTF::Model ModelImporter::import(const std::filesystem::path& path) {
    m_path = path;

    glTF::Model model;

    std::ifstream ifs{path, std::ios::binary};
    if (!(ifs.is_open() && ifs.good())) {
        throw Exception{std::format("Failed to open asset file: {}", path.string())};
    }

    Header header;
    ifs.read(reinterpret_cast<char*>(&header), sizeof(header));
    ifs.seekg(0);

    if (header.magic == HEADER_MAGIC) {
        parseGLB(model, ifs);
    } else {
        parseGLTF(model, ifs);
    }

    populateModel(model);

    LOG_INFO("=== Extensions Used ===");
    for (auto& extension : model.root.extensionsUsed) {
        LOG_INFO("\t- {}", extension);
    }

    LOG_INFO("=== Extensions Required ===");
    for (auto& extension : model.root.extensionsRequired) {
        LOG_INFO("\t- {}", extension);
    }

    return model; // nrvo
}

void ModelImporter::parseGLB(glTF::Model& model, std::ifstream& ifs) {
    Header header;
    ifs.read(reinterpret_cast<char*>(&header), sizeof(header));

    R3_ASSERT(header.magic == HEADER_MAGIC && "GLB files must have magic number");

    if (header.version > R3_GLB_VERSION) {
        LOG_WARNING("glb version for {} is {} while R3 supports up to glb version {}",
                    m_path.string(),
                    header.version,
                    R3_GLB_VERSION);
    }

    ChunkHeader chunkHeader;

    auto readJson = [&] {
        std::string jsonFile(chunkHeader.length, '\0');
        ifs.read(jsonFile.data(), chunkHeader.length);
        model.document.Parse(jsonFile.data(), chunkHeader.length);
    };

    auto readBin = [&] {
        auto& buf = model.bin.emplace_back();
        buf.resize(chunkHeader.length);
        ifs.read(reinterpret_cast<char*>(buf.data()), chunkHeader.length);
    };

    /* glb files have 2 chunks */
    for (int i = 0; i < 2; i++) {
        ifs.read(reinterpret_cast<char*>(&chunkHeader), sizeof(chunkHeader));
        if (chunkHeader.type == CHUNK_TYPE_JSON) {
            readJson();
        } else if (chunkHeader.type == CHUNK_TYPE_BIN) {
            readBin();
        } else {
            throw Exception("invalid chunk header type");
        }
    }
}

void ModelImporter::parseGLTF(glTF::Model& model, std::ifstream& ifs) {
    std::string json = (std::stringstream() << ifs.rdbuf()).str();
    model.document.Parse(json.c_str());
}

void ModelImporter::populateModel(glTF::Model& model) {
    populateExtensionsUsed(model);
    populateExtensionsRequired(model);
    populateAccessors(model);
    populateAnimations(model);
    populateAsset(model);
    populateBuffers(model, model.bin);
    populateBufferViews(model);
    populateCameras(model);
    populateImages(model);
    populateMaterials(model);
    populateMeshes(model);
    populateNodes(model);
    populateSamplers(model);
    populateScene(model);
    populateScenes(model);
    populateSkins(model);
    populateTextures(model);
    populateExtensions(model);
    populateExtras(model);
}

void ModelImporter::populateExtensionsUsed(glTF::Model& model) {
    if (!model.document.HasMember("extensionsUsed")) {
        return;
    }

    for (auto& extension : model.document["extensionsUsed"].GetArray()) {
        model.root.extensionsUsed.emplace_back(extension.GetString());
    }
}

void ModelImporter::populateExtensionsRequired(glTF::Model& model) {
    if (!model.document.HasMember("extensionsRequired")) {
        return;
    }

    for (auto& extension : model.document["extensionsRequired"].GetArray()) {
        model.root.extensionsRequired.emplace_back(extension.GetString());
    }
}

void ModelImporter::populateAccessors(glTF::Model& model) {
    if (!model.document.HasMember("accessors")) {
        return;
    }

    for (auto& itAccessor : model.document["accessors"].GetArray()) {
        Accessor& accessor = model.root.accessors.emplace_back();
        // bufferView
        maybeAssign(accessor.bufferView, itAccessor, "bufferView");
        // byteOffset
        maybeAssign(accessor.byteOffset, itAccessor, "byteOffset");
        // componentType
        accessor.componentType = itAccessor["componentType"].GetUint();
        // normalized
        maybeAssign(accessor.normalized, itAccessor, "normalized");
        // count
        accessor.count = itAccessor["count"].GetUint();
        // type
        accessor.type = itAccessor["type"].GetString();
        // max
        if (itAccessor.HasMember("max")) {
            for (auto& elem : itAccessor["max"].GetArray()) {
                accessor.max.push_back(elem.GetFloat());
            }
        }
        // min
        if (itAccessor.HasMember("min")) {
            for (auto& elem : itAccessor["min"].GetArray()) {
                accessor.max.push_back(elem.GetFloat());
            }
        }
        // sparse
        if (itAccessor.HasMember("sparse")) {
            LOG_WARNING("TODO Sparse Accessors");
        }
        // name
        maybeAssign(accessor.name, itAccessor, "name");
        // extensions
        maybeMove(accessor.extensions, itAccessor, "extensions");
        // extras
        maybeMove(accessor.extras, itAccessor, "extras");
    }
}

void ModelImporter::populateAnimations(glTF::Model& model) {
    if (!model.document.HasMember("animations")) {
        return;
    }

    for (auto& itAnimation : model.document["animations"].GetArray()) {
        Animation& animation = model.root.animations.emplace_back();
        // channels
        for (auto& itChannel : itAnimation["channels"].GetArray()) {
            AnimationChannel& channel = animation.channels.emplace_back();
            // sampler
            channel.sampler = itChannel["sampler"].GetUint();
            // target
            {
                auto& jsTarget = itChannel["target"];
                // node
                maybeAssign(channel.target.node, jsTarget, "node");
                // path
                channel.target.path = jsTarget["path"].GetString();
                // extensions
                maybeMove(channel.extensions, jsTarget, "extensions");
                // extras
                maybeMove(channel.extras, jsTarget, "extras");
            }
            // extensions
            maybeMove(channel.extensions, itChannel, "extensions");
            // extras
            maybeMove(channel.extras, itChannel, "extras");
        }
        // samplers
        for (auto& itSampler : itAnimation["samplers"].GetArray()) {
            AnimationSampler& sampler = animation.samplers.emplace_back();
            // input
            sampler.input = itSampler["input"].GetUint();
            // interpolation
            maybeAssign(sampler.interpolation, itSampler, "interpolation");
            // output
            sampler.output = itSampler["output"].GetUint();
            // extensions
            maybeMove(sampler.extensions, itSampler, "extensions");
            // extras
            maybeMove(sampler.extras, itSampler, "extras");
        }
        // name
        maybeAssign(animation.name, itAnimation, "name");
        // extensions
        maybeMove(animation.extensions, itAnimation, "extensions");
        // extras
        maybeMove(animation.extras, itAnimation, "extras");
    }
}

void ModelImporter::populateAsset(glTF::Model& model) {
    auto& jsAsset = model.document["asset"];
    // copyright -- ignore
    // generator -- ignore
    // version
    model.root.asset.version = jsAsset["version"].GetString();
    checkVersion(model.root.asset.version);
    // minVersion
    maybeAssign(model.root.asset.minVersion, jsAsset, "minVersion");
    // extensions
    maybeMove(model.root.asset.extensions, jsAsset, "extensions");
    // extras
    maybeMove(model.root.asset.extras, jsAsset, "extras");
}
void ModelImporter::populateBuffers(glTF::Model& model, std::vector<std::vector<std::byte>>& bin) {
    if (!model.document.HasMember("buffers")) {
        return;
    }

    for (auto& itBuffer : model.document["buffers"].GetArray()) {
        Buffer& buffer = model.root.buffers.emplace_back();
        // uri
        maybeAssign(buffer.uri, itBuffer, "uri");
        // byteLength
        buffer.byteLength = itBuffer["byteLength"].GetUint();
        // name
        maybeAssign(buffer.name, itBuffer, "name");
        // extensions
        maybeMove(buffer.extensions, itBuffer, "extensions");
        // extras
        maybeMove(buffer.extras, itBuffer, "extras");
        /* load in buffer if external file */
        if (!buffer.uri.empty()) {
            std::ifstream ifs;
            ifs.exceptions(std::ifstream::badbit);

            std::filesystem::path bufferPath = m_path;
            bufferPath.replace_filename(buffer.uri);
            ifs.open(bufferPath, std::ios::binary);

            auto& buf = bin.emplace_back(buffer.byteLength);
            ifs.read(reinterpret_cast<char*>(buf.data()), buf.size());
        }
    }
}

void ModelImporter::populateBufferViews(glTF::Model& model) {
    if (!model.document.HasMember("bufferViews")) {
        return;
    }

    for (auto& itBufferView : model.document["bufferViews"].GetArray()) {
        BufferView& bufferView = model.root.bufferViews.emplace_back();
        // buffer
        bufferView.buffer = itBufferView["buffer"].GetUint();
        // byteOffset
        maybeAssign(bufferView.byteOffset, itBufferView, "byteOffset");
        // byteLength
        bufferView.byteLength = itBufferView["byteLength"].GetUint();
        // byteStride
        maybeAssign(bufferView.byteStride, itBufferView, "byteStride");
        // target
        maybeAssign(bufferView.target, itBufferView, "target");
        // name
        maybeAssign(bufferView.name, itBufferView, "name");
        // extensions
        maybeMove(bufferView.extensions, itBufferView, "extensions");
        // extras
        maybeMove(bufferView.extras, itBufferView, "extras");
    }
}

void ModelImporter::populateCameras(glTF::Model& model) {
    if (!model.document.HasMember("cameras")) {
        return;
    }

    LOG_WARNING("TODO cameras");
}

void ModelImporter::populateImages(glTF::Model& model) {
    if (!model.document.HasMember("images"))
        return;

    for (auto& itImage : model.document["images"].GetArray()) {
        Image& image = model.root.images.emplace_back();
        // uri
        maybeAssign(image.uri, itImage, "uri");
        // mimeType
        maybeAssign(image.mimeType, itImage, "mimeType");
        // bufferView
        maybeAssign(image.bufferView, itImage, "bufferView");
        // name
        maybeAssign(image.name, itImage, "name");
        // extensions
        maybeMove(image.extensions, itImage, "extensions");
        // extras
        maybeMove(image.extras, itImage, "extras");
    }
}

void ModelImporter::populateMaterials(glTF::Model& model) {
    if (!model.document.HasMember("materials")) {
        return;
    }

    for (auto& itMaterial : model.document["materials"].GetArray()) {
        Material& material = model.root.materials.emplace_back();
        // pbrMetallicRoughness
        if (itMaterial.HasMember("pbrMetallicRoughness")) {
            auto& jsPbr                                = itMaterial["pbrMetallicRoughness"];
            material.pbrMetallicRoughness              = PBRMetallicRoughness{};
            PBRMetallicRoughness& pbrMetallicRoughness = *material.pbrMetallicRoughness;
            // baseColorFactor
            if (jsPbr.HasMember("baseColorFactor")) {
                for (uint32 i = 0; auto& baseColorFactor : jsPbr["baseColorFactor"].GetArray()) {
                    pbrMetallicRoughness.baseColorFactor.at(i++) = baseColorFactor.GetFloat();
                }
            }
            // baseColorTexture
            if (jsPbr.HasMember("baseColorTexture")) {
                pbrMetallicRoughness.baseColorTexture = TextureInfo{};
                populateTextureInfo(*pbrMetallicRoughness.baseColorTexture, jsPbr["baseColorTexture"]);
            }
            // metallicFactor
            maybeAssign(pbrMetallicRoughness.metallicFactor, jsPbr, "metallicFactor");
            // roughnessFactor
            maybeAssign(pbrMetallicRoughness.roughnessFactor, jsPbr, "roughnessFactor");
            // metallicRoughnessTexture
            if (jsPbr.HasMember("metallicRoughnessTexture")) {
                pbrMetallicRoughness.metallicRoughnessTexture = TextureInfo{};
                populateTextureInfo(*pbrMetallicRoughness.metallicRoughnessTexture, jsPbr["metallicRoughnessTexture"]);
            }
            // extensions
            maybeMove(pbrMetallicRoughness.extensions, jsPbr, "extensions");
            // extras
            maybeMove(pbrMetallicRoughness.extras, jsPbr, "extras");
        }
        // normalTexture
        if (itMaterial.HasMember("normalTexture")) {
            auto& jsNormal                   = itMaterial["normalTexture"];
            material.normalTexture           = NormalTextureInfo{};
            NormalTextureInfo& normalTexture = *material.normalTexture;
            // index
            normalTexture.index = jsNormal["index"].GetUint();
            // texCoord
            maybeAssign(normalTexture.texCoord, jsNormal, "texCoord");
            // scale
            maybeAssign(normalTexture.scale, jsNormal, "scale");
            // extensions
            maybeMove(normalTexture.extensions, jsNormal, "extensions");
            // extras
            maybeMove(normalTexture.extras, jsNormal, "extras");
        }
        // occlusionTexture
        if (itMaterial.HasMember("occlusionTexture")) {
            auto& jsOcclusion                      = itMaterial["occlusionTexture"];
            material.occlusionTexture              = OcclusionTextureInfo{};
            OcclusionTextureInfo& occlusionTexture = *material.occlusionTexture;
            // index
            occlusionTexture.index = jsOcclusion["index"].GetUint();
            // texCoord
            maybeAssign(occlusionTexture.texCoord, jsOcclusion, "texCoord");
            // strength
            maybeAssign(occlusionTexture.strength, jsOcclusion, "strength");
            // extensions
            maybeMove(occlusionTexture.extensions, jsOcclusion, "extensions");
            // extras
            maybeMove(occlusionTexture.extras, jsOcclusion, "extras");
        }
        // emissiveTexture
        if (itMaterial.HasMember("emissiveTexture")) {
            auto& jsEmissive             = itMaterial["emissiveTexture"];
            material.emissiveTexture     = TextureInfo{};
            TextureInfo& emissiveTexture = *material.emissiveTexture;
            populateTextureInfo(emissiveTexture, jsEmissive);
        }
        // emissiveFactor
        if (itMaterial.HasMember("emissiveFactor")) {
            for (uint32 i = 0; auto& itEmissiveFactor : itMaterial["emissiveFactor"].GetArray()) {
                material.emissiveFactor.at(i++) = itEmissiveFactor.GetFloat();
            }
        }
        // alphaMode
        maybeAssign(material.alphaMode, itMaterial, "alphaMode");
        // alphaCutoff
        maybeAssign(material.alphaCutoff, itMaterial, "alphaCutoff");
        // doubleSided
        maybeAssign(material.doubleSided, itMaterial, "doubleSided");
        // name
        maybeAssign(material.name, itMaterial, "name");
        // extensions
        if (itMaterial.HasMember("extensions")) {
            auto& jsExtensions = itMaterial["extensions"];
#if 0 // (TODO) extension support
            if (jsExtensions.HasMember(EXTENSION_KHR_materials_pbrSpecularGlossiness)) {
                material.extensions.emplace_back(new KHR_materials_pbrSpecularGlossiness);
                KHR_materials_pbrSpecularGlossiness::parse(material.extensions.back().get(),
                                                           jsExtensions[EXTENSION_KHR_materials_pbrSpecularGlossiness]);
            }
#else
            (void)jsExtensions;
#endif
        }
        // extras
        maybeMove(material.extras, itMaterial, "extras");
    }
}

void ModelImporter::populateMeshes(glTF::Model& model) {
    if (!model.document.HasMember("meshes")) {
        return;
    }

    for (auto& itMesh : model.document["meshes"].GetArray()) {
        Mesh& mesh = model.root.meshes.emplace_back();
        // primitives
        for (auto& itPrimitive : itMesh["primitives"].GetArray()) {
            MeshPrimitive& primitive = mesh.primitives.emplace_back();
            // attributes
            for (auto& [key, value] : itPrimitive["attributes"].GetObject()) {
                primitive.attributes.emplace(key.GetString(), value.GetUint());
            }
            // indices
            maybeAssign(primitive.indices, itPrimitive, "indices");
            // material
            maybeAssign(primitive.material, itPrimitive, "material");
            // mode
            maybeAssign(primitive.mode, itPrimitive, "mode");
            // targets
            if (itPrimitive.HasMember("targets")) {
                for (auto& itTarget : itPrimitive["targets"].GetArray()) {
                    auto& target = primitive.targets.emplace_back();
                    for (auto& [key, value] : itTarget.GetObject()) {
                        target.emplace(key.GetString(), value.GetUint());
                    }
                }
            }
            // extensions
            maybeMove(primitive.extensions, itPrimitive, "extensions");
            // extras
            maybeMove(primitive.extras, itPrimitive, "extras");
        }
        // weights
        if (itMesh.HasMember("weights")) {
            for (auto& itWeight : itMesh["weights"].GetArray()) {
                mesh.weights.push_back(itWeight.GetFloat());
            }
        }
        // name
        maybeAssign(mesh.name, itMesh, "name");
        // extensions
        maybeMove(mesh.extensions, itMesh, "extensions");
        // extras
        maybeMove(mesh.extras, itMesh, "extras");
    }
}

void ModelImporter::populateNodes(glTF::Model& model) {
    if (!model.document.HasMember("nodes")) {
        return;
    }

    for (auto& itNode : model.document["nodes"].GetArray()) {
        Node& node = model.root.nodes.emplace_back();
        // camera
        maybeAssign(node.camera, itNode, "camera");
        // children
        if (itNode.HasMember("children")) {
            for (auto& itChild : itNode["children"].GetArray()) {
                node.children.push_back(itChild.GetUint());
            }
        }
        // skin
        maybeAssign(node.skin, itNode, "skin");
        // matrix
        if (itNode.HasMember("matrix")) {
            for (uint32 i = 0; auto& itMatrix : itNode["matrix"].GetArray()) {
                node.matrix.at(i++) = itMatrix.GetFloat();
            }
        }
        // mesh
        maybeAssign(node.mesh, itNode, "mesh");
        // rotation
        if (itNode.HasMember("rotation")) {
            for (uint32 i = 0; auto& itRotation : itNode["rotation"].GetArray()) {
                node.rotation.at(i++) = itRotation.GetFloat();
            }
        }
        // scale
        if (itNode.HasMember("scale")) {
            for (uint32 i = 0; auto& itScale : itNode["scale"].GetArray()) {
                node.scale.at(i++) = itScale.GetFloat();
            }
        }
        // translation
        if (itNode.HasMember("translation")) {
            for (uint32 i = 0; auto& itTranslation : itNode["translation"].GetArray()) {
                node.translation.at(i++) = itTranslation.GetFloat();
            }
        }
        // weights
        if (itNode.HasMember("weights")) {
            for (auto& itWeight : itNode["weights"].GetArray()) {
                node.weights.push_back(itWeight.GetFloat());
            }
        }
        // name
        maybeAssign(node.name, itNode, "name");
        // extensions
        maybeMove(node.extensions, itNode, "extensions");
        // extras
        maybeMove(node.extras, itNode, "extras");
    }
}

void ModelImporter::populateSamplers(glTF::Model& model) {
    if (!model.document.HasMember("samplers")) {
        return;
    }

    for (auto& itSampler : model.document["samplers"].GetArray()) {
        Sampler& sampler = model.root.samplers.emplace_back();
        // magFilter
        maybeAssign(sampler.magFilter, itSampler, "magFilter");
        // minFilter
        maybeAssign(sampler.minFilter, itSampler, "minFilter");
        // wrapS
        maybeAssign(sampler.wrapS, itSampler, "wrapS");
        // wrapT
        maybeAssign(sampler.wrapT, itSampler, "wrapT");
        // name
        maybeAssign(sampler.name, itSampler, "name");
        // extensions
        maybeMove(sampler.extensions, itSampler, "extensions");
        // extras
        maybeMove(sampler.extras, itSampler, "extras");
    }
}

void ModelImporter::populateScene(glTF::Model& model) {
    maybeAssign(model.root.scene, model.document, "scene");
}

void ModelImporter::populateScenes(glTF::Model& model) {
    if (!model.document.HasMember("scenes")) {
        return;
    }

    for (auto& itScene : model.document["scenes"].GetArray()) {
        Scene& nthScene = model.root.scenes.emplace_back();
        // nodes
        if (itScene.HasMember("nodes")) {
            for (auto& itNode : itScene["nodes"].GetArray()) {
                nthScene.nodes.push_back(itNode.GetUint());
            }
        }
        // name
        maybeAssign(nthScene.name, itScene, "name");
        // extensions
        maybeMove(nthScene.extensions, itScene, "extensions");
        // extras
        maybeMove(nthScene.extras, itScene, "extras");
    }
}

void ModelImporter::populateSkins(glTF::Model& model) {
    if (!model.document.HasMember("skins")) {
        return;
    }

    for (auto& itSkin : model.document["skins"].GetArray()) {
        Skin& skin = model.root.skins.emplace_back();
        // inverseBindMatrices
        maybeAssign(skin.inverseBindMatrices, itSkin, "inverseBindMatrices");
        // skeleton
        maybeAssign(skin.skeleton, itSkin, "skeleton");
        // joints
        for (auto& itJoint : itSkin["joints"].GetArray()) {
            skin.joints.push_back(itJoint.GetUint());
        }
        // name
        maybeAssign(skin.name, itSkin, "name");
        // extensions
        maybeMove(skin.extensions, itSkin, "extensions");
        // extras
        maybeMove(skin.extras, itSkin, "extras");
    }
}

void ModelImporter::populateTextures(glTF::Model& model) {
    if (!model.document.HasMember("textures")) {
        return;
    }

    for (auto& itTexture : model.document["textures"].GetArray()) {
        Texture& texture = model.root.textures.emplace_back();
        // sampler
        maybeAssign(texture.sampler, itTexture, "sampler");
        // source
        maybeAssign(texture.source, itTexture, "source");
        // name
        maybeAssign(texture.name, itTexture, "name");
        // extensions
        maybeMove(texture.extensions, itTexture, "extensions");
        // extras
        maybeMove(texture.extras, itTexture, "extras");
    }
}

void ModelImporter::populateExtensions(glTF::Model& model) {
    if (!model.document.HasMember("extensions")) {
        return;
    }

    model.root.extensions = model.document["extensions"].Move();
}

void ModelImporter::populateExtras(glTF::Model& model) {
    if (!model.document.HasMember("extras")) {
        return;
    }

    model.root.extras = model.document["extras"].Move();
}

void ModelImporter::checkVersion(std::string_view version) const {
    char* end;
    uint32 major = strtol(version.data(), &end, 10);
    uint32 minor = strtol(end, NULL, 10);
    checkVersion(major, minor);
}

void ModelImporter::checkVersion(uint32 major, uint32 minor) const {
    if (major < R3_GLTF_VERSION_MAJOR) {
        return;
    }

    if ((major == R3_GLTF_VERSION_MAJOR) && (minor <= R3_GLTF_VERSION_MAJOR)) {
        return;
    }

    std::string assetVersion      = std::format("{}.{}", major, minor);
    std::string engineGltfVersion = std::format("{}.{}", R3_GLTF_VERSION_MAJOR, R3_GLTF_VERSION_MAJOR);
    LOG_WARNING("glTF version of asset {} is {} while R3 support up to glTF version {}",
                m_path.string(),
                assetVersion,
                engineGltfVersion);
}

template <typename T, typename U>
constexpr bool is_same_or_optional_v = (std::is_same_v<T, U> || std::is_same_v<T, std::optional<U>>);

template <typename T>
void ModelImporter::maybeAssign(T& dst, const json::Value& value, const char* key) {
    if (!value.HasMember(key)) {
        return;
    }

    if constexpr (is_same_or_optional_v<T, bool>) {
        dst = value[key].GetBool();
    } else if constexpr (is_same_or_optional_v<T, uint32>) {
        dst = value[key].GetUint();
    } else if constexpr (is_same_or_optional_v<T, int32>) {
        dst = value[key].GetInt();
    } else if constexpr (is_same_or_optional_v<T, uint64>) {
        dst = value[key].GetUint64();
    } else if constexpr (is_same_or_optional_v<T, int64>) {
        dst = value[key].GetInt64();
    } else if constexpr (is_same_or_optional_v<T, float>) {
        dst = value[key].GetFloat();
    } else if constexpr (is_same_or_optional_v<T, double>) {
        dst = value[key].GetDouble();
    } else if constexpr (is_same_or_optional_v<T, std::string>) {
        dst = value[key].GetString();
    }
}

void ModelImporter::maybeMove(json::Value& dst, json::Value& value, const char* key) {
    if (!value.HasMember(key)) {
        return;
    }

    dst = value[key].Move();
}

// TextureInfo helper
void ModelImporter::populateTextureInfo(TextureInfo& textureInfo, json::Value& value) {
    // index
    textureInfo.index = value["index"].GetUint();
    // texCoord
    maybeAssign(textureInfo.texCoord, value, "texCoord");
    // extensions
    maybeMove(textureInfo.extensions, value, "extensions");
    // extras
    maybeMove(textureInfo.extras, value, "extras");
}

} // namespace R3::glTF
#include "GLTF_Model.hxx"
#include <fstream>
#include "api/Check.hpp"
#include "api/Ensure.hpp"
#include "api/Log.hpp"
#include "api/Todo.hpp"
#include "api/Version.hpp"

using namespace rapidjson;

namespace R3 {

template <typename T>
static constexpr void maybeAssign(T& dst, const Value& value, const char* key) {
    if (!value.HasMember(key))
        return;

    if constexpr (std::is_same_v<T, bool>) {
        dst = value[key].GetBool();
    } else if constexpr (std::is_same_v<T, uint32>) {
        dst = value[key].GetUint();
    } else if constexpr (std::is_same_v<T, int32>) {
        dst = value[key].GetInt();
    } else if constexpr (std::is_same_v<T, uint64>) {
        dst = value[key].GetUint64();
    } else if constexpr (std::is_same_v<T, int64>) {
        dst = value[key].GetInt64();
    } else if constexpr (std::is_same_v<T, float>) {
        dst = value[key].GetFloat();
    } else if constexpr (std::is_same_v<T, double>) {
        dst = value[key].GetDouble();
    } else if constexpr (std::is_same_v<T, std::string>) {
        dst = value[key].GetString();
    }
}

GLTF_Model::GLTF_Model(std::string_view path)
    : m_path(path) {
    std::ifstream ifs(path.data(), std::ios::binary);
    ENSURE(ifs.is_open() && ifs.good());

    // GLTF-BIN HEADER
    GLTF_Header header = {};
    ifs.read((char*)(&header), sizeof(header));

    ENSURE(header.magic == GLTF_HEADER_MAGIC);
    if (header.version > GLB_VERSION) {
        LOG(Warning, "glb version for", path, "is", header.version, "while R3 supports glb version", GLB_VERSION);
    }

    // GLTF-BIN CHUNK
    GLTF_ChunkHeader chunkHeader = {};
    ifs.read((char*)(&chunkHeader), sizeof(chunkHeader));
    ENSURE(chunkHeader.type == GLTF_CHUNK_TYPE_JSON);

    std::string jsonFile(chunkHeader.length, '\0');
    ifs.read(jsonFile.data(), chunkHeader.length);

    m_document.Parse(jsonFile.c_str());

    ifs.read((char*)(&chunkHeader), sizeof(chunkHeader));
    ENSURE(chunkHeader.type == GLTF_CHUNK_TYPE_BIN);
    m_buffer.resize(chunkHeader.length);
    ifs.read(m_buffer.data(), chunkHeader.length);

    populateRoot();
}

void GLTF_Model::populateRoot() {
    populateExtensionsUsed();
    populateExtensionsRequired();
    populateAccessors();
    populateAnimations();
    populateAsset();
    populateBuffers();
    populateBufferViews();
    populateCameras();
    populateImages();
    populateMaterials();
    populateMeshes();
    populateNodes();
    populateSamplers();
    populateScene();
    populateScenes();
    populateSkins();
    populateTextures();
    populateExtensions();
    populateExtras();
}

void GLTF_Model::populateExtensionsUsed() {
    if (!m_document.HasMember("extensionsUsed"))
        return;

    for (auto& extension : m_document["extensionsUsed"].GetArray())
        extensionsUsed.emplace_back(extension.GetString());
}

void GLTF_Model::populateExtensionsRequired() {
    if (!m_document.HasMember("extensionsRequired"))
        return;

    for (auto& extension : m_document["extensionsRequired"].GetArray())
        extensionsRequired.emplace_back(extension.GetString());
}

void GLTF_Model::populateAccessors() {
    if (!m_document.HasMember("accessors"))
        return;

    for (auto& itAccessor : m_document["accessors"].GetArray()) {
        GLTF_Accessor& accessor = accessors.emplace_back();

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
            for (auto& elem : itAccessor.GetObject()["max"].GetArray())
                accessor.max.push_back(elem.GetFloat());
        }

        // min
        if (itAccessor.HasMember("min")) {
            for (auto& elem : itAccessor.GetObject()["min"].GetArray())
                accessor.max.push_back(elem.GetFloat());
        }

        // sparse
        if (itAccessor.HasMember("sparse"))
            TODO("sparse incomplete");

        // name
        maybeAssign(accessor.name, itAccessor, "name");

        // extensions
        if (itAccessor.HasMember("extensions"))
            accessor.extensions = std::move(itAccessor["extensions"]);

        // extras
        if (itAccessor.HasMember("extras"))
            accessor.extras = std::move(itAccessor["extras"]);
    }
}

void GLTF_Model::populateAnimations() {
    if (!m_document.HasMember("animations"))
        return;

    LOG(Warning, "TODO animations");
}

void GLTF_Model::populateAsset() {
    auto& jsAsset = m_document["asset"];

    // copyright -- ignore

    // generator -- ignore

    // version
    asset.version = jsAsset["version"].GetString();
    checkVersion(asset.version);

    // minVersion
    maybeAssign(asset.minVersion, jsAsset, "minVersion");

    // extensions
    if (jsAsset.HasMember("extensions"))
        asset.extensions = std::move(jsAsset["extensions"]);

    // extras
    if (jsAsset.HasMember("extras"))
        asset.extras = std::move(jsAsset["extras"]);
}

void GLTF_Model::populateBuffers() {
    if (!m_document.HasMember("buffers"))
        return;

    for (auto& itBuffer : m_document["buffers"].GetArray()) {
        GLTF_Buffer& buffer = buffers.emplace_back();

        // uri
        maybeAssign(buffer.uri, itBuffer, "uri");

        // byteLength
        maybeAssign(buffer.byteLength, itBuffer, "byteLength");

        // name
        maybeAssign(buffer.name, itBuffer, "name");

        // extensions
        if (itBuffer.HasMember("extensions"))
            buffer.extensions = std::move(itBuffer["extensions"]);

        // extras
        if (itBuffer.HasMember("extras"))
            buffer.extras = std::move(itBuffer["extras"]);
    }
}

void GLTF_Model::populateBufferViews() {
    if (!m_document.HasMember("bufferViews"))
        return;

    for (auto& itBufferView : m_document["bufferViews"].GetArray()) {
        GLTF_BufferView& bufferView = bufferViews.emplace_back();

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
        if (itBufferView.HasMember("extensions"))
            bufferView.extensions = std::move(itBufferView["extensions"]);

        // extras
        if (itBufferView.HasMember("extras"))
            bufferView.extras = std::move(itBufferView["extras"]);
    }
}

void GLTF_Model::populateCameras() {
    if (!m_document.HasMember("cameras"))
        return;

    LOG(Warning, "TODO cameras");
}

void GLTF_Model::populateImages() {
    if (!m_document.HasMember("images"))
        return;

    for (auto& itImage : m_document["images"].GetArray()) {
        GLTF_Image& image = images.emplace_back();

        // uri
        maybeAssign(image.uri, itImage, "uri");

        // mimeType
        maybeAssign(image.mimeType, itImage, "mimeType");

        // bufferView
        maybeAssign(image.bufferView, itImage, "bufferView");

        // name
        maybeAssign(image.name, itImage, "name");

        // extensions
        if (itImage.HasMember("extensions"))
            image.extensions = std::move(itImage["extensions"]);

        // extras
        if (itImage.HasMember("extras"))
            image.extras = std::move(itImage["extras"]);
    }
}

void GLTF_Model::populateMaterials() {
    if (!m_document.HasMember("materials"))
        return;

    for (auto& itMaterial : m_document["materials"].GetArray()) {
        GLTF_Material& material = materials.emplace_back();

        // pbrMetallicRoughness
        if (itMaterial.HasMember("pbrMetallicRoughness")) {
            auto& jsPbr = itMaterial["pbrMetallicRoughness"];
            material.pbrMetallicRoughness = GLTF_PBRMetallicRoughness{};
            GLTF_PBRMetallicRoughness& pbrMetallicRoughness = *material.pbrMetallicRoughness;

            // baseColorFactor
            if (jsPbr.HasMember("baseColorFactor")) {
                for (uint32 i = 0; auto& baseColorFactor : jsPbr["baseColorFactor"].GetArray())
                    pbrMetallicRoughness.baseColorFactor[i++] = baseColorFactor.GetFloat();
            }

            // baseColorTexture
            if (jsPbr.HasMember("baseColorTexture")) {
                pbrMetallicRoughness.baseColorTexture = GLTF_TextureInfo{};
                populateTextureInfo(*pbrMetallicRoughness.baseColorTexture, jsPbr["baseColorTexture"]);
            }

            // metallicFactor
            maybeAssign(pbrMetallicRoughness.metallicFactor, jsPbr, "metallicFactor");

            // roughnessFactor
            maybeAssign(pbrMetallicRoughness.roughnessFactor, jsPbr, "roughnessFactor");

            // metallicRoughnessTexture
            if (jsPbr.HasMember("metallicRoughnessTexture")) {
                pbrMetallicRoughness.metallicRoughnessTexture = GLTF_TextureInfo{};
                populateTextureInfo(*pbrMetallicRoughness.metallicRoughnessTexture, jsPbr["metallicRoughnessTexture"]);
            }

            // extensions
            if (jsPbr.HasMember("extensions"))
                pbrMetallicRoughness.metallicRoughnessTexture->extensions = std::move(jsPbr["extensions"]);

            // extras
            if (jsPbr.HasMember("extras"))
                pbrMetallicRoughness.metallicRoughnessTexture->extras = std::move(jsPbr["extras"]);
        }

        // normalTexture
        if (itMaterial.HasMember("normalTexture")) {
            auto& jsNormal = itMaterial["normalTexture"];
            material.normalTexture = GLTF_NormalTextureInfo{};
            GLTF_NormalTextureInfo& normalTexture = *material.normalTexture;

            // index
            normalTexture.index = jsNormal["index"].GetUint();

            // texCoord
            maybeAssign(normalTexture.texCoord, jsNormal, "texCoord");

            // scale
            maybeAssign(normalTexture.scale, jsNormal, "scale");

            // extensions
            if (jsNormal.HasMember("extensions"))
                normalTexture.extensions = std::move(jsNormal["extensions"]);;

            // extras
            if (jsNormal.HasMember("extras"))
                normalTexture.extras = std::move(jsNormal["extras"]);
        }

        // occlusionTexture
        if (itMaterial.HasMember("occlusionTexture")) {
            auto& jsOcclusion = itMaterial["occlusionTexture"];
            material.occlusionTexture = GLTF_OcclusionTextureInfo{};
            GLTF_OcclusionTextureInfo& occlusionTexture = *material.occlusionTexture;

            // index
            occlusionTexture.index = jsOcclusion["index"].GetUint();

            // texCoord
            maybeAssign(occlusionTexture.texCoord, jsOcclusion, "texCoord");

            // strength
            maybeAssign(occlusionTexture.strength, jsOcclusion, "strength");

            // extensions
            if (jsOcclusion.HasMember("extensions"))
                occlusionTexture.extensions = std::move(jsOcclusion["extensions"]);;

            // extras
            if (jsOcclusion.HasMember("extras"))
                occlusionTexture.extras = std::move(jsOcclusion["extras"]);
        }

        // emissiveTexture
        if (itMaterial.HasMember("emissiveTexture")) {
            auto& jsEmissive = itMaterial["emissiveTexture"];
            material.emissiveTexture = GLTF_TextureInfo{};
            GLTF_TextureInfo& emissiveTexture = *material.emissiveTexture;

            populateTextureInfo(emissiveTexture, jsEmissive);
        }

        // emissiveFactor
        if (itMaterial.HasMember("emissiveFactor")) {
            for (uint32 i = 0; auto& itEmissiveFactor : itMaterial["emissiveFactor"].GetArray())
                material.emissiveFactor[i++] = itEmissiveFactor.GetFloat();
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
        if (itMaterial.HasMember("extensions"))
            material.extensions = std::move(itMaterial["extensions"]);

        // extras
        if (itMaterial.HasMember("extras"))
            material.extras = std::move(itMaterial["extras"]);
    }
}

void GLTF_Model::populateMeshes() {
    if (!m_document.HasMember("meshes"))
        return;

    for (auto& itMesh : m_document["meshes"].GetArray()) {
        GLTF_Mesh& mesh = meshes.emplace_back();

        // primitives
        for (auto& itPrimitive : itMesh["primitives"].GetArray()) {
            GLTF_MeshPrimitive& primitive = mesh.primitives.emplace_back();

            // attributes
            primitive.attributes = std::move(itPrimitive["attributes"]);

            // indices
            maybeAssign(primitive.indices, itPrimitive, "indices");

            // material
            maybeAssign(primitive.material, itPrimitive, "material");

            // mode
            maybeAssign(primitive.mode, itPrimitive, "mode");

            // targets
            if (itPrimitive.HasMember("targets")) {
                for (auto& itTarget : itPrimitive["targets"].GetArray())
                    primitive.targets.emplace_back(std::move(itTarget.GetObject()));
            }

            // extensions
            if (itPrimitive.HasMember("extensions"))
                primitive.extensions = std::move(itPrimitive["extensions"]);

            // extras
            if (itPrimitive.HasMember("extras"))
                primitive.extras = std::move(itPrimitive["extras"]);
        }

        // weights
        if (itMesh.HasMember("weights")) {
            for (auto& itWeight : itMesh["weights"].GetArray())
                mesh.weights.push_back(itWeight.GetFloat());
        }

        // name
        maybeAssign(mesh.name, itMesh, "name");

        // extensions
        if (itMesh.HasMember("extensions"))
            mesh.extensions = std::move(itMesh["extensions"]);

        // extras
        if (itMesh.HasMember("extras"))
            mesh.extras = std::move(itMesh["extras"]);
    }
}

void GLTF_Model::populateNodes() {
    if (!m_document.HasMember("nodes"))
        return;

    for (auto& itNode : m_document["nodes"].GetArray()) {
        GLTF_Node& node = nodes.emplace_back();

        // camera
        maybeAssign(node.camera, itNode, "camera");

        // children
        if (itNode.HasMember("children")) {
            for (auto& itChild : itNode["children"].GetArray())
                node.children.push_back(itChild.GetUint());
        }

        // skin
        maybeAssign(node.skin, itNode, "skin");

        // matrix
        if (itNode.HasMember("matrix")) {
            for (uint32 i = 0; auto& itMatrix : itNode["matrix"].GetArray())
                node.matrix[i++] = itMatrix.GetFloat();
        }

        // mesh
        maybeAssign(node.mesh, itNode, "mesh");

        // rotation
        if (itNode.HasMember("rotation")) {
            for (uint32 i = 0; auto& itRotation : itNode["rotation"].GetArray())
                node.rotation[i++] = itRotation.GetFloat();
        }

        // scale
        if (itNode.HasMember("scale")) {
            for (uint32 i = 0; auto& itScale : itNode["scale"].GetArray())
                node.scale[i++] = itScale.GetFloat();
        }

        // translation
        if (itNode.HasMember("translation")) {
            for (uint32 i = 0; auto& itTranslation : itNode["translation"].GetArray())
                node.translation[i++] = itTranslation.GetFloat();
        }

        // weights
        if (itNode.HasMember("weights")) {
            for (auto& itWeight : itNode["weights"].GetArray())
                node.weights.push_back(itWeight.GetFloat());
        }

        // name
        maybeAssign(node.name, itNode, "name");

        // extensions
        if (itNode.HasMember("extensions"))
            node.extensions = std::move(itNode["extensions"]);

        // extras
        if (itNode.HasMember("extras"))
            node.extras = std::move(itNode["extras"]);
    }
}

void GLTF_Model::populateSamplers() {
    if (!m_document.HasMember("samplers"))
        return;

    for (auto& itSampler : m_document["samplers"].GetArray()) {
        GLTF_Sampler& sampler = samplers.emplace_back();

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
        if (itSampler.HasMember("extensions"))
            sampler.extensions = std::move(itSampler["extensions"]);

        // extras
        if (itSampler.HasMember("extras"))
            sampler.extras = std::move(itSampler["extras"]);
    }
}

void GLTF_Model::populateScene() {
    maybeAssign(scene, m_document, "scene");
}

void GLTF_Model::populateScenes() {
    if (!m_document.HasMember("scenes"))
        return;

    for (auto& itScene : m_document["scenes"].GetArray()) {
        GLTF_Scene& nthScene = scenes.emplace_back();

        // nodes
        if (itScene.HasMember("nodes")) {
            for (auto& itNode : itScene["nodes"].GetArray())
                nthScene.nodes.push_back(itNode.GetUint());
        }

        // name
        maybeAssign(nthScene.name, itScene, "name");

        // extensions
        if (itScene.HasMember("extensions"))
            nthScene.extensions = std::move(itScene["extensions"]);

        // extras
        if (itScene.HasMember("extras"))
            nthScene.extras = std::move(itScene["extras"]);
    }
}

void GLTF_Model::populateSkins() {
    if (!m_document.HasMember("skins"))
        return;

    for (auto& itSkin : m_document["skins"].GetArray()) {
        GLTF_Skin& skin = skins.emplace_back();

        // inverseBindMatrices
        maybeAssign(skin.inverseBindMatrices, itSkin, "inverseBindMatrices");

        // skeleton
        maybeAssign(skin.skeleton, itSkin, "skeleton");

        // joints
        for (auto& itJoint : itSkin["joints"].GetArray())
            skin.joints.push_back(itJoint.GetUint());

        // name
        maybeAssign(skin.name, itSkin, "name");

        // extensions
        if (itSkin.HasMember("extensions"))
            skin.extensions = std::move(itSkin["extensions"]);

        // extras
        if (itSkin.HasMember("extras"))
            skin.extras = std::move(itSkin["extras"]);
    }
}

void GLTF_Model::populateTextures() {
    if (!m_document.HasMember("textures"))
        return;

    for (auto& itTexture : m_document["textures"].GetArray()) {
        GLTF_Texture& texture = textures.emplace_back();

        // sampler
        maybeAssign(texture.sampler, itTexture, "sampler");

        // source
        maybeAssign(texture.source, itTexture, "source");

        // name
        maybeAssign(texture.name, itTexture, "name");

        // extensions
        if (itTexture.HasMember("extensions"))
            texture.extensions = std::move(itTexture["extensions"]);

        // extras
        if (itTexture.HasMember("extras"))
            texture.extras = std::move(itTexture["extras"]);
    }
}

void GLTF_Model::populateExtensions() {
    if (!m_document.HasMember("extensions"))
        return;

    extensions = std::move(m_document["extensions"]);
}

void GLTF_Model::populateExtras() {
    if (!m_document.HasMember("extras"))
        return;

    extras = std::move(m_document["extras"]);
}

void GLTF_Model::populateTextureInfo(GLTF_TextureInfo& textureInfo, rapidjson::Value& value) {
    // index
    textureInfo.index = value["index"].GetUint();

    // texCoord
    maybeAssign(textureInfo.texCoord, value, "texCoord");

    // extensions
    if (value.HasMember("extensions"))
        textureInfo.extensions = std::move(value["extensions"]);

    // extras
    if (value.HasMember("extras"))
        textureInfo.extras = std::move(value["extras"]);
}

void GLTF_Model::checkVersion(std::string_view version) const {
    char* end;
    uint32 major = strtol(version.data(), &end, 10);
    uint32 minor = strtol(end, NULL, 10);
    checkVersion(major, minor);
}

void GLTF_Model::checkVersion(uint32 major, uint32 minor) const {
    if (major > R3::GLTF_VERSION_MAJOR || (major == R3::GLTF_VERSION_MAJOR && minor > R3::GLTF_VERSION_MINOR)) {
        std::string assetVersion = (std::stringstream() << major << "." << minor).str();
        std::string engineGltfVersion = (std::stringstream() << GLTF_VERSION_MAJOR << "." << GLTF_VERSION_MINOR).str();
        LOG(Warning, "glTF version of asset", m_path, "is", assetVersion,
            "while R3 supports glTF version", engineGltfVersion);
    }
}

} // namespace R3
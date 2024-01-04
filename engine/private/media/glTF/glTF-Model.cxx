#include "glTF-Model.hxx"

#include <sstream>
#include "api/Ensure.hpp"
#include "api/Log.hpp"
#include "api/Version.hpp"
#include "glTF-Extensions.hxx"
#include "glTF-Util.hxx"

using namespace rapidjson;

namespace R3::glTF {

Model::Model(std::string_view path)
    : m_path(path) {
    std::ifstream ifs(path.data(), std::ios::binary);
    ENSURE(ifs.is_open() && ifs.good());

    bool glbSuccess = parseGLB(ifs);
    bool gltfSuccess = parseGLTF(ifs);

    CHECK(glbSuccess || gltfSuccess);

    populateRoot();

    LOG(Verbose, "--- Extensions Used ---");
    for (auto& extension : extensionsUsed) {
        LOG(Verbose, "\t-", extension);
    }

    LOG(Verbose, "--- Extensions Required ---");
    for (auto& extension : extensionsRequired) {
        LOG(Verbose, "\t-", extension);
    }
}

bool Model::parseGLB(std::ifstream& ifs) {
    Header header = {};
    ifs.read((char*)(&header), sizeof(header));

    if (header.magic != HEADER_MAGIC) {
        ifs.seekg(0);
        return false; // early exit, not glb file
    }

    if (header.version > GLB_VERSION) {
        LOG(Warning, "glb version for", m_path, "is", header.version, "while R3 supports glb version", GLB_VERSION);
    }

    ChunkHeader chunkHeader = {};

    auto readJson = [&]() {
        std::string jsonFile(chunkHeader.length, '\0');
        ifs.read(jsonFile.data(), chunkHeader.length);
        m_document.Parse(jsonFile.c_str());
    };

    auto readBin = [&]() {
        m_buffer.resize(chunkHeader.length);
        ifs.read((char*)m_buffer.data(), chunkHeader.length);
    };

    ifs.read((char*)(&chunkHeader), sizeof(chunkHeader));
    if (chunkHeader.type == CHUNK_TYPE_JSON) {
        readJson();
    } else if (chunkHeader.type == CHUNK_TYPE_BIN) {
        readBin();
    } else {
        LOG(Error, "invalid chunk header type");
        ENSURE(false);
    }

    ifs.read((char*)(&chunkHeader), sizeof(chunkHeader));
    if (chunkHeader.type == CHUNK_TYPE_JSON) {
        readJson();
    } else if (chunkHeader.type == CHUNK_TYPE_BIN) {
        readBin();
    } else {
        LOG(Error, "invalid chunk header type");
        ENSURE(false);
    }

    return true;
}

bool Model::parseGLTF(std::ifstream& ifs) {
    std::string json = (std::stringstream() << ifs.rdbuf()).str();
    m_document.Parse(json.c_str());
    return true;
}

void Model::populateRoot() {
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

void Model::populateExtensionsUsed() {
    if (!m_document.HasMember("extensionsUsed")) {
        return;
    }

    for (auto& extension : m_document["extensionsUsed"].GetArray()) {
        extensionsUsed.emplace_back(extension.GetString());
    }
}

void Model::populateExtensionsRequired() {
    if (!m_document.HasMember("extensionsRequired")) {
        return;
    }

    for (auto& extension : m_document["extensionsRequired"].GetArray()) {
        extensionsRequired.emplace_back(extension.GetString());
    }
}

void Model::populateAccessors() {
    if (!m_document.HasMember("accessors")) {
        return;
    }

    for (auto& itAccessor : m_document["accessors"].GetArray()) {
        Accessor& accessor = accessors.emplace_back();

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
            for (auto& elem : itAccessor.GetObject()["max"].GetArray()) {
                accessor.max.push_back(elem.GetFloat());
            }
        }

        // min
        if (itAccessor.HasMember("min")) {
            for (auto& elem : itAccessor.GetObject()["min"].GetArray()) {
                accessor.max.push_back(elem.GetFloat());
            }
        }

        // sparse
        if (itAccessor.HasMember("sparse")) {
            ENSURE(false);
        }

        // name
        maybeAssign(accessor.name, itAccessor, "name");

        // extensions
        if (itAccessor.HasMember("extensions")) {
            accessor.extensions = std::move(itAccessor["extensions"]);
        }

        // extras
#if R3_GLTF_JSON_EXTRAS
        if (itAccessor.HasMember("extras")) {
            accessor.extras = std::move(itAccessor["extras"]);
        }
#endif
    }
}

void Model::populateAnimations() {
    if (!m_document.HasMember("animations")) {
        return;
    }

    for (auto& itAnimation : m_document["animations"].GetArray()) {
        Animation& animation = animations.emplace_back();

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
                if (jsTarget.HasMember("extensions")) {
                    channel.extensions = std::move(jsTarget["extensions"]);
                }

                // extras
#if R3_GLTF_JSON_EXTRAS
                if (jsTarget.HasMember("extras")) {
                    channel.extras = std::move(jsTarget["extras"]);
                }
#endif
            }

            // extensions
            if (itChannel.HasMember("extensions")) {
                channel.extensions = std::move(itChannel["extensions"]);
            }

            // extras
#if R3_GLTF_JSON_EXTRAS
            if (itChannel.HasMember("extras")) {
                channel.extras = std::move(itChannel["extras"]);
            }
#endif
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
            if (itSampler.HasMember("extensions")) {
                sampler.extensions = std::move(itSampler["extensions"]);
            }

            // extras
#if R3_GLTF_JSON_EXTRAS
            if (itSampler.HasMember("extras")) {
                sampler.extras = std::move(itSampler["extras"]);
            }
#endif
        }

        // name
        maybeAssign(animation.name, itAnimation, "name");

        // extensions
        if (itAnimation.HasMember("extensions")) {
            animation.extensions = std::move(itAnimation["extensions"]);
        }

        // extras
#if R3_GLTF_JSON_EXTRAS
        if (itAnimation.HasMember("extras")) {
            animation.extras = std::move(itAnimation["extras"]);
        }
#endif
    }
}

void Model::populateAsset() {
    auto& jsAsset = m_document["asset"];

    // copyright -- ignore

    // generator -- ignore

    // version
    asset.version = jsAsset["version"].GetString();
    checkVersion(asset.version);

    // minVersion
    maybeAssign(asset.minVersion, jsAsset, "minVersion");

    // extensions
    if (jsAsset.HasMember("extensions")) {
        asset.extensions = std::move(jsAsset["extensions"]);
    }

    // extras
#if R3_GLTF_JSON_EXTRAS
    if (jsAsset.HasMember("extras")) {
        asset.extras = std::move(jsAsset["extras"]);
    }
#endif
}

void Model::populateBuffers() {
    if (!m_document.HasMember("buffers")) {
        return;
    }

    for (auto& itBuffer : m_document["buffers"].GetArray()) {
        Buffer& buffer = buffers.emplace_back();

        // uri
        maybeAssign(buffer.uri, itBuffer, "uri");

        // byteLength
        buffer.byteLength = itBuffer["byteLength"].GetUint();

        // name
        maybeAssign(buffer.name, itBuffer, "name");

        // extensions
        if (itBuffer.HasMember("extensions")) {
            buffer.extensions = std::move(itBuffer["extensions"]);
        }

        // extras
#if R3_GLTF_JSON_EXTRAS
        if (itBuffer.HasMember("extras")) {
            buffer.extras = std::move(itBuffer["extras"]);
        }
#endif

        /* load in buffer if external file */
        if (!buffer.uri.empty()) {
            usize split = m_path.find_last_of('/') + 1;
            std::string dir = m_path.substr(0, split);

            std::ifstream ifs;
            ifs.exceptions(std::ifstream::badbit);

            try {
                ifs.open(dir + buffer.uri, std::ios::binary);
                m_buffer.resize(buffer.byteLength);
                ifs.read((char*)m_buffer.data(), buffer.byteLength);
            } catch (std::exception& e) {
                LOG(Error, "glTF buffer read error", e.what());
            }
        }
    }
}

void Model::populateBufferViews() {
    if (!m_document.HasMember("bufferViews")) {
        return;
    }

    for (auto& itBufferView : m_document["bufferViews"].GetArray()) {
        BufferView& bufferView = bufferViews.emplace_back();

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
        if (itBufferView.HasMember("extensions")) {
            bufferView.extensions = std::move(itBufferView["extensions"]);
        }

        // extras
#if R3_GLTF_JSON_EXTRAS
        if (itBufferView.HasMember("extras")) {
            bufferView.extras = std::move(itBufferView["extras"]);
        }
#endif
    }
}

void Model::populateCameras() {
    if (!m_document.HasMember("cameras")) {
        return;
    }

    LOG(Warning, "TODO cameras");
}

void Model::populateImages() {
    if (!m_document.HasMember("images"))
        return;

    for (auto& itImage : m_document["images"].GetArray()) {
        Image& image = images.emplace_back();

        // uri
        maybeAssign(image.uri, itImage, "uri");

        // mimeType
        maybeAssign(image.mimeType, itImage, "mimeType");

        // bufferView
        maybeAssign(image.bufferView, itImage, "bufferView");

        // name
        maybeAssign(image.name, itImage, "name");

        // extensions
        if (itImage.HasMember("extensions")) {
            image.extensions = std::move(itImage["extensions"]);
        }

        // extras
#if R3_GLTF_JSON_EXTRAS
        if (itImage.HasMember("extras")) {
            image.extras = std::move(itImage["extras"]);
        }
#endif
    }
}

void Model::populateMaterials() {
    if (!m_document.HasMember("materials")) {
        return;
    }

    for (auto& itMaterial : m_document["materials"].GetArray()) {
        Material& material = materials.emplace_back();

        // pbrMetallicRoughness
        if (itMaterial.HasMember("pbrMetallicRoughness")) {
            auto& jsPbr = itMaterial["pbrMetallicRoughness"];
            material.pbrMetallicRoughness = PBRMetallicRoughness{};
            PBRMetallicRoughness& pbrMetallicRoughness = material.pbrMetallicRoughness;

            // baseColorFactor
            if (jsPbr.HasMember("baseColorFactor")) {
                for (uint32 i = 0; auto& baseColorFactor : jsPbr["baseColorFactor"].GetArray()) {
                    pbrMetallicRoughness.baseColorFactor[i++] = baseColorFactor.GetFloat();
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
            if (jsPbr.HasMember("extensions")) {
                pbrMetallicRoughness.metallicRoughnessTexture->extensions = std::move(jsPbr["extensions"]);
            }

            // extras
#if R3_GLTF_JSON_EXTRAS
            if (jsPbr.HasMember("extras")) {
                pbrMetallicRoughness.metallicRoughnessTexture->extras = std::move(jsPbr["extras"]);
            }
#endif
        }

        // normalTexture
        if (itMaterial.HasMember("normalTexture")) {
            auto& jsNormal = itMaterial["normalTexture"];
            material.normalTexture = NormalTextureInfo{};
            NormalTextureInfo& normalTexture = *material.normalTexture;

            // index
            normalTexture.index = jsNormal["index"].GetUint();

            // texCoord
            maybeAssign(normalTexture.texCoord, jsNormal, "texCoord");

            // scale
            maybeAssign(normalTexture.scale, jsNormal, "scale");

            // extensions
            if (jsNormal.HasMember("extensions")) {
                normalTexture.extensions = std::move(jsNormal["extensions"]);
            }

            // extras
#if R3_GLTF_JSON_EXTRAS
            if (jsNormal.HasMember("extras")) {
                normalTexture.extras = std::move(jsNormal["extras"]);
            }
#endif
        }

        // occlusionTexture
        if (itMaterial.HasMember("occlusionTexture")) {
            auto& jsOcclusion = itMaterial["occlusionTexture"];
            material.occlusionTexture = OcclusionTextureInfo{};
            OcclusionTextureInfo& occlusionTexture = *material.occlusionTexture;

            // index
            occlusionTexture.index = jsOcclusion["index"].GetUint();

            // texCoord
            maybeAssign(occlusionTexture.texCoord, jsOcclusion, "texCoord");

            // strength
            maybeAssign(occlusionTexture.strength, jsOcclusion, "strength");

            // extensions
            if (jsOcclusion.HasMember("extensions")) {
                occlusionTexture.extensions = std::move(jsOcclusion["extensions"]);
            }

            // extras
#if R3_GLTF_JSON_EXTRAS
            if (jsOcclusion.HasMember("extras")) {
                occlusionTexture.extras = std::move(jsOcclusion["extras"]);
            }
#endif
        }

        // emissiveTexture
        if (itMaterial.HasMember("emissiveTexture")) {
            auto& jsEmissive = itMaterial["emissiveTexture"];
            material.emissiveTexture = TextureInfo{};
            TextureInfo& emissiveTexture = *material.emissiveTexture;

            populateTextureInfo(emissiveTexture, jsEmissive);
        }

        // emissiveFactor
        if (itMaterial.HasMember("emissiveFactor")) {
            for (uint32 i = 0; auto& itEmissiveFactor : itMaterial["emissiveFactor"].GetArray()) {
                material.emissiveFactor[i++] = itEmissiveFactor.GetFloat();
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
            if (jsExtensions.HasMember(EXTENSION_KHR_materials_pbrSpecularGlossiness)) {
                material.extensions.emplace_back(new KHR_materials_pbrSpecularGlossiness);
                KHR_materials_pbrSpecularGlossiness::parse(material.extensions.back().get(),
                                                           jsExtensions[EXTENSION_KHR_materials_pbrSpecularGlossiness]);
            }
        }

        // extras
#if R3_GLTF_JSON_EXTRAS
        if (itMaterial.HasMember("extras")) {
            material.extras = std::move(itMaterial["extras"]);
        }
#endif
    }
}

void Model::populateMeshes() {
    if (!m_document.HasMember("meshes")) {
        return;
    }

    for (auto& itMesh : m_document["meshes"].GetArray()) {
        Mesh& mesh = meshes.emplace_back();

        // primitives
        for (auto& itPrimitive : itMesh["primitives"].GetArray()) {
            MeshPrimitive& primitive = mesh.primitives.emplace_back();

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
                for (auto& itTarget : itPrimitive["targets"].GetArray()) {
                    primitive.targets.emplace_back(itTarget.GetObject());
                }
            }

            // extensions
            if (itPrimitive.HasMember("extensions")) {
                primitive.extensions = std::move(itPrimitive["extensions"]);
            }

            // extras
#if R3_GLTF_JSON_EXTRAS
            if (itPrimitive.HasMember("extras")) {
                primitive.extras = std::move(itPrimitive["extras"]);
            }
#endif
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
        if (itMesh.HasMember("extensions")) {
            mesh.extensions = std::move(itMesh["extensions"]);
        }

        // extras
#if R3_GLTF_JSON_EXTRAS
        if (itMesh.HasMember("extras")) {
            mesh.extras = std::move(itMesh["extras"]);
        }
#endif
    }
}

void Model::populateNodes() {
    if (!m_document.HasMember("nodes")) {
        return;
    }

    for (auto& itNode : m_document["nodes"].GetArray()) {
        Node& node = nodes.emplace_back();

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
                node.matrix[i++] = itMatrix.GetFloat();
            }
        }

        // mesh
        maybeAssign(node.mesh, itNode, "mesh");

        // rotation
        if (itNode.HasMember("rotation")) {
            for (uint32 i = 0; auto& itRotation : itNode["rotation"].GetArray()) {
                node.rotation[i++] = itRotation.GetFloat();
            }
        }

        // scale
        if (itNode.HasMember("scale")) {
            for (uint32 i = 0; auto& itScale : itNode["scale"].GetArray()) {
                node.scale[i++] = itScale.GetFloat();
            }
        }

        // translation
        if (itNode.HasMember("translation")) {
            for (uint32 i = 0; auto& itTranslation : itNode["translation"].GetArray()) {
                node.translation[i++] = itTranslation.GetFloat();
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
        if (itNode.HasMember("extensions")) {
            node.extensions = std::move(itNode["extensions"]);
        }

        // extras
#if R3_GLTF_JSON_EXTRAS
        if (itNode.HasMember("extras")) {
            node.extras = std::move(itNode["extras"]);
        }
#endif
    }
}

void Model::populateSamplers() {
    if (!m_document.HasMember("samplers")) {
        return;
    }

    for (auto& itSampler : m_document["samplers"].GetArray()) {
        Sampler& sampler = samplers.emplace_back();

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
        if (itSampler.HasMember("extensions")) {
            sampler.extensions = std::move(itSampler["extensions"]);
        }

        // extras
#if R3_GLTF_JSON_EXTRAS
        if (itSampler.HasMember("extras")) {
            sampler.extras = std::move(itSampler["extras"]);
        }
#endif
    }
}

void Model::populateScene() {
    maybeAssign(scene, m_document, "scene");
}

void Model::populateScenes() {
    if (!m_document.HasMember("scenes")) {
        return;
    }

    for (auto& itScene : m_document["scenes"].GetArray()) {
        Scene& nthScene = scenes.emplace_back();

        // nodes
        if (itScene.HasMember("nodes")) {
            for (auto& itNode : itScene["nodes"].GetArray()) {
                nthScene.nodes.push_back(itNode.GetUint());
            }
        }

        // name
        maybeAssign(nthScene.name, itScene, "name");

        // extensions
        if (itScene.HasMember("extensions")) {
            nthScene.extensions = std::move(itScene["extensions"]);
        }

        // extras
#if R3_GLTF_JSON_EXTRAS
        if (itScene.HasMember("extras")) {
            nthScene.extras = std::move(itScene["extras"]);
        }
#endif
    }
}

void Model::populateSkins() {
    if (!m_document.HasMember("skins")) {
        return;
    }

    for (auto& itSkin : m_document["skins"].GetArray()) {
        Skin& skin = skins.emplace_back();

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
        if (itSkin.HasMember("extensions")) {
            skin.extensions = std::move(itSkin["extensions"]);
        }

        // extras
#if R3_GLTF_JSON_EXTRAS
        if (itSkin.HasMember("extras")) {
            skin.extras = std::move(itSkin["extras"]);
        }
#endif
    }
}

void Model::populateTextures() {
    if (!m_document.HasMember("textures")) {
        return;
    }

    for (auto& itTexture : m_document["textures"].GetArray()) {
        Texture& texture = textures.emplace_back();

        // sampler
        maybeAssign(texture.sampler, itTexture, "sampler");

        // source
        maybeAssign(texture.source, itTexture, "source");

        // name
        maybeAssign(texture.name, itTexture, "name");

        // extensions
        if (itTexture.HasMember("extensions")) {
            texture.extensions = std::move(itTexture["extensions"]);
        }

        // extras
#if R3_GLTF_JSON_EXTRAS
        if (itTexture.HasMember("extras")) {
            texture.extras = std::move(itTexture["extras"]);
        }
#endif
    }
}

void Model::populateExtensions() {
    if (!m_document.HasMember("extensions")) {
        return;
    }

    extensions = std::move(m_document["extensions"]);
}

void Model::populateExtras() {
#if R3_GLTF_JSON_EXTRAS
    if (!m_document.HasMember("extras")) {
        return;
    }

    extras = std::move(m_document["extras"]);
#endif
}

void Model::checkVersion(std::string_view version) const {
    char* end;
    uint32 major = strtol(version.data(), &end, 10);
    uint32 minor = strtol(end, NULL, 10);
    checkVersion(major, minor);
}

void Model::checkVersion(uint32 major, uint32 minor) const {
    if (major > R3::GLTF_VERSION_MAJOR || (major == R3::GLTF_VERSION_MAJOR && minor > R3::GLTF_VERSION_MINOR)) {
        std::string assetVersion = (std::stringstream() << major << "." << minor).str();
        std::string engineGltfVersion = (std::stringstream() << GLTF_VERSION_MAJOR << "." << GLTF_VERSION_MINOR).str();
        LOG(Warning,
            "glTF version of asset",
            m_path,
            "is",
            assetVersion,
            "while R3 supports glTF version",
            engineGltfVersion);
    }
}

} // namespace R3::glTF
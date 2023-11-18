#include "GLTF_Model.hxx"
#include <fstream>
#include "api/Check.hpp"
#include "api/Log.hpp"
#include "api/Todo.hpp"
#include "api/Version.hpp"

using namespace rapidjson;

namespace R3 {

template<typename T>
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
    CHECK(ifs.is_open() && ifs.good());

    // GLTF-BIN HEADER
    GLTF_Header header = {};
    ifs.read((char*)(&header), sizeof(header));

    CHECK(header.magic == GLTF_HEADER_MAGIC);
    if (header.version > GLB_VERSION) {
        LOG(Warning, "glb version for", path, "is", header.version, "while R3 supports glb version", GLB_VERSION);
    }

    // GLTF-BIN CHUNK
    GLTF_ChunkHeader chunkHeader = {};
    ifs.read((char*)(&chunkHeader), sizeof(chunkHeader));
    CHECK(chunkHeader.type == GLTF_CHUNK_TYPE_JSON);

    std::string jsonFile(chunkHeader.length, '\0');
    ifs.read(jsonFile.data(), chunkHeader.length);

    m_document.Parse(jsonFile.c_str());

    ifs.read((char*)(&chunkHeader), sizeof(chunkHeader));
    CHECK(chunkHeader.type == GLTF_CHUNK_TYPE_BIN);
    m_buffer.resize(chunkHeader.length);
    ifs.read(m_buffer.data(), chunkHeader.length);

    populateRoot();
}

void GLTF_Model::populateRoot() {
    // extensionsUsed
    if (m_document.HasMember("extensionsUsed")) {
        for (auto& extension : m_document["extensionsUsed"].GetArray())
            extensionsUsed.emplace_back(extension.GetString());
    }

    // extensionsRequired
    if (m_document.HasMember("extensionsRequired")) {
        for (auto& extension : m_document["extensionsRequired"].GetArray())
            extensionsRequired.emplace_back(extension.GetString());
    }

    // accessors
    if (m_document.HasMember("accessors")) {
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

    // animations
    if (m_document.HasMember("animations")) {
        LOG(Warning, "TODO animations");
    }

    // asset
    {
        auto& asset_ = m_document["asset"];

        // copyright -- ignore

        // generator -- ignore

        // version
        asset.version = asset_["version"].GetString();
        checkVersion(asset.version);

        // minVersion
        maybeAssign(asset.minVersion, asset_, "minVersion");

        // extensions
        if (asset_.HasMember("extensions"))
            asset.extensions = std::move(asset_["extensions"]);

        // extras
        if (asset_.HasMember("extras"))
            asset.extras = std::move(asset_["extras"]);
    }

    // buffers
    if (m_document.HasMember("buffers")) {
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

    // bufferViews
    if (m_document.HasMember("bufferViews")) {
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

    // cameras
    if (m_document.HasMember("cameras")) {
        LOG(Warning, "TODO cameras");
    }

    // images
    if (m_document.HasMember("images")) {
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

    // materials
    if (m_document.HasMember("materials")) {
        LOG(Warning, "TODO materials");
    }

    // meshes
    if (m_document.HasMember("meshes")) {
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

    // nodes
    if (m_document.HasMember("nodes")) {
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

    // samplers
    if (m_document.HasMember("samplers")) {
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

    // scene
    maybeAssign(scene, m_document, "scene");

    // scenes
    if (m_document.HasMember("scenes")) {
        for (auto& itScene : m_document["scenes"].GetArray()) {
            GLTF_Scene& scene_ = scenes.emplace_back();

            // nodes
            if (itScene.HasMember("nodes")) {
                for (auto& itNode : itScene["nodes"].GetArray())
                    scene_.nodes.push_back(itNode.GetUint());
            }

            // name
            maybeAssign(scene_.name, itScene, "name");

            // extensions
            if (itScene.HasMember("extensions"))
                scene_.extensions = std::move(itScene["extensions"]);

            // extras
            if (itScene.HasMember("extras"))
                scene_.extras = std::move(itScene["extras"]);
        }
    }

    // skins
    if (m_document.HasMember("skins")) {
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

    // textures
    if (m_document.HasMember("textures")) {
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

    // extensions
    if (m_document.HasMember("extensions"))
        extensions = std::move(m_document["extensions"]);

    // extras
    if (m_document.HasMember("extras"))
        extras = std::move(m_document["extras"]);
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
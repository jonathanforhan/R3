#include "ModelImporter.hxx"
#include <fstream>
#include "api/Check.hpp"
#include "api/Log.hpp"
#include "api/Version.hpp"
#include "detail/spec/glTF.hxx"

using namespace rapidjson;

namespace R3 {

ModelImporter::ModelImporter(std::string_view path) {
    usize split = path.find_last_of('/') + 1;
    m_directory = path.substr(0, split);
    m_file = path.substr(split);

    std::ifstream bifs(path.data(), std::ios::binary);
    CHECK(bifs.is_open() && bifs.good());

    //--- GLTF-BIN HEADER ---//
    GLTF_Header header = {};
    bifs.read((char*)(&header), sizeof(header));

    CHECK(header.magic == GLTF_HEADER_MAGIC);
    if (header.version > GLB_VERSION) {
        LOG(Warning, "glb version for", path, "is", header.version, "while R3 supports glb version", GLB_VERSION);
    }

    //--- GLTF-BIN CHUNK ---//
    GLTF_ChunkHeader chunkHeader = {};
    bifs.read((char*)(&chunkHeader), sizeof(chunkHeader));
    CHECK(chunkHeader.type == GLTF_CHUNK_TYPE_JSON);

    std::string jsonFile(chunkHeader.length, '\0');
    bifs.read(jsonFile.data(), chunkHeader.length);

    bifs.read((char*)(&chunkHeader), sizeof(chunkHeader));
    CHECK(chunkHeader.type == GLTF_CHUNK_TYPE_BIN);

    m_document.Parse(jsonFile.c_str());
    checkVersion(m_document["asset"]["version"].GetString());

    m_buffer.resize(chunkHeader.length);
    bifs.read(m_buffer.data(), chunkHeader.length);

    if (m_document.HasMember("meshes")) {
        m_meshes.resize(m_document["meshes"].GetArray().Size());
    }

    for (auto it = m_document["scenes"].Begin(); it != m_document["scenes"].End(); ++it) {
        if (it->HasMember("nodes")) {
            for (auto nit = (*it)["nodes"].Begin(); nit != (*it)["nodes"].End(); ++nit) {
                processNode(nit->GetUint());
            }
        }
    }
}

void ModelImporter::processNode(uint32 index) {
    auto& node = m_document["nodes"].GetArray()[index];
    if (node.HasMember("children")) {
        for (auto chit = node["children"].Begin(); chit != node["children"].End(); ++chit) {
            processNode(chit->GetUint());
        }
    }

    if (node.HasMember("mesh")) {
        processMesh(node["mesh"].GetUint());
    }
}

void ModelImporter::processMesh(uint32 index) {
    auto& mesh = m_document["meshes"][index];
    auto& attributes = mesh["primitives"][0]["attributes"];
    auto& accessors = m_document["accessors"];
    auto& bufferViews = m_document["bufferViews"];
    auto& buffers = m_document["buffers"];

    uint32 position = attributes[GLTF_POSITION].GetUint();

    int32 bufferView = accessors[position].HasMember("bufferView") ? accessors[position]["bufferView"].GetUint() : -1;
    uint32 byteOffset = accessors[position].HasMember("byteOffset") ? accessors[position]["byteOffset"].GetUint() : 0;
    bool normalized = accessors[position].HasMember("normalized") ? accessors[position]["normalized"].GetBool() : false;
    uint32 count = accessors[position]["count"].GetUint();
    uint32 componentType = accessors[position]["componentType"].GetUint();
    std::string type = accessors[position]["type"].GetString();

    uint32 buffer = bufferViews[bufferView]["buffer"].GetUint();
    uint32 bufferByteOffset =
        bufferViews[bufferView].HasMember("byteOffset") ? bufferViews[bufferView]["byteOffset"].GetUint() : 0;
    uint32 byteLength = bufferViews[bufferView]["byteLength"].GetUint();

    m_meshes[index].positions.resize(count);
    memcpy(m_meshes[index].positions.data(), m_buffer.data() + byteOffset + bufferByteOffset, count * sizeof(float));

    // if normals


}

void ModelImporter::checkVersion(std::string_view version) const {
    char* end;
    uint32 major = strtol(version.data(), &end, 10);
    uint32 minor = strtol(end, NULL, 10);
    checkVersion(major, minor);
}

void ModelImporter::checkVersion(uint32 major, uint32 minor) const {
    if (major > R3::GLTF_VERSION_MAJOR || (major == R3::GLTF_VERSION_MAJOR && minor > R3::GLTF_VERSION_MINOR)) {
        std::string assetVersion = (std::stringstream() << major << "." << minor).str();
        std::string engineGltfVersion = (std::stringstream() << GLTF_VERSION_MAJOR << "." << GLTF_VERSION_MINOR).str();
        LOG(Warning, "glTF version of asset", m_directory + m_file, "is", assetVersion,
            "while R3 supports glTF version", engineGltfVersion);
    }
}

} // namespace R3
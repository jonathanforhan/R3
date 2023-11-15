#include "ModelComponent.hpp"
#include "ModelComponent.hpp"
#include "ModelComponent.hpp"
#include <rapidjson/document.h>
#include <rapidjson/istreamwrapper.h>
#include <fstream>
#include "api/Check.hpp"
#include "api/Log.hpp"
#include "api/Version.hpp"
#include "core/Engine.hpp"

namespace R3 {

namespace json {
using namespace rapidjson;
}

constexpr auto GLB_MAGIC = 0x46546C67;

ModelComponent::ModelComponent(const std::string& path, Shader& shader)
    : m_shader(shader),
      m_directory(),
      m_file() {
    usize split = path.find_last_of('/') + 1;
    m_directory = path.substr(0, split);
    m_file = path.substr(split);

    std::ifstream bifs(path, std::ios::binary);
    CHECK(bifs.is_open() && bifs.good());

    uint32 magic = 0;
    bifs.read((char*)(&magic), sizeof(magic));
    CHECK(magic == GLB_MAGIC);

    uint32 version = 0;
    bifs.read((char*)(&version), sizeof(version));
    if (version > GLB_VERSION) {
        LOG(Warning, "glb container version for", path, "is", version, "while R3 supports glb container version",
            GLB_VERSION);
    }

    uint32 length = 0;
    bifs.read((char*)(&length), sizeof(length));

    json::Document document;

    // std::string version = document["asset"]["version"].GetString();
    // checkVersion(version);
}

void ModelComponent::checkVersion(std::string_view version) const {
    char* end;
    uint32 major = strtol(version.data(), &end, 10);
    uint32 minor = strtol(end, NULL, 10);
    checkVersion(major, minor);
}

void ModelComponent::checkVersion(uint32 major, uint32 minor) const {
    if (major > R3::GLTF_VERSION_MAJOR || (major == R3::GLTF_VERSION_MAJOR && minor > R3::GLTF_VERSION_MINOR)) {
        std::string assetVersion = (std::stringstream() << major << "." << minor).str();
        std::string engineGltfVersion = (std::stringstream() << GLTF_VERSION_MAJOR << "." << GLTF_VERSION_MINOR).str();
        LOG(Warning, "glTF version of asset", m_directory + m_file, "is", assetVersion,
            "while R3 supports glTF version", engineGltfVersion);
    }
}

} // namespace R3
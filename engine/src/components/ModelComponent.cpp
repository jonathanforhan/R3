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

ModelComponent::ModelComponent(const std::string& path, Shader& shader)
    : m_shader(shader),
      m_directory(),
      m_file() {
    usize split = path.find_last_of('/') + 1;
    m_directory = path.substr(0, split);
    m_file = path.substr(split);
    LOG(Info, m_directory, m_file);

    json::Document document;

    std::string s;
    std::ifstream ifs;
    ifs.exceptions(std::ifstream::badbit);

    try {
        ifs.open(path);

        json::IStreamWrapper stream(ifs);
        document.ParseStream(stream);

        ifs.close();
    } catch(std::exception& e) {
        LOG(Error, "model import error", path, e.what());
        return;
    }

    CHECK(document.HasMember("asset") && document["asset"].HasMember("version"));
    std::string version = document["asset"]["version"].GetString();
    checkVersion(version);

    std::string binFilename= document["buffers"][0]["uri"].GetString();
    LOG(Info, "opening", binFilename);

    std::ifstream binFile = std::ifstream(binFilename, std::ios::binary | std::ios::ate);
    binFile.exceptions(std::ifstream::badbit);
    CHECK(binFile.is_open());

    usize binLen = binFile.tellg();
    binFile.seekg(0);

    std::vector<char> bin(binLen);
    binFile.read(bin.data(), binLen);
    binFile.close();
}

void ModelComponent::checkVersion(std::string_view version) {
    char* end;
    uint32 major = strtol(version.data(), &end, 10);
    uint32 minor = strtol(end, NULL, 10);
    checkVersion(major, minor);
}

void ModelComponent::checkVersion(uint32 major, uint32 minor) {
    if (major > R3::GLTF_VERSION_MAJOR || (major == R3::GLTF_VERSION_MAJOR && minor > R3::GLTF_VERSION_MINOR)) {
        std::string assetVersion = (std::stringstream() << major << "." << minor).str();
        std::string engineGltfVersion = (std::stringstream() << GLTF_VERSION_MAJOR << "." << GLTF_VERSION_MINOR).str();
        LOG(Warning, "glTF version of asset", path, "is", assetVersion, "while R3 supports glTF version",
            engineGltfVersion);
    }
}

} // namespace R3
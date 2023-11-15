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

    char* end;
    uint32 major = strtol(version.data(), &end, 10), minor = strtol(end, NULL, 10);

    if (major > R3::GLTF_VERSION_MAJOR || (major == R3::GLTF_VERSION_MAJOR && minor > R3::GLTF_VERSION_MINOR)) {
        LOG(Warning, "glTF version of asset", path, "is", version, "while R3 supports glTF version",
            std::to_string(GLTF_VERSION_MAJOR) + "." + std::to_string(GLTF_VERSION_MINOR));
    }

    if (!document.HasMember("scene")) {
        return;
    }
}

} // namespace R3
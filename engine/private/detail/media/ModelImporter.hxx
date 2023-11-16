#pragma once
#include <rapidjson/document.h>
#include <string_view>
#include <vector>
#include "api/Types.hpp"

namespace R3 {

struct ImportMesh {
    std::vector<float> positions;
    std::vector<float> normals;
    std::vector<uint32> indices;
};

// Only supports glTF
class ModelImporter {
public:
    explicit ModelImporter(std::string_view path);

    auto meshes() const -> const std::vector<ImportMesh>& { return m_meshes; }

private:
    void processNode(uint32 index);
    void processMesh(uint32 index);

    void checkVersion(std::string_view version) const;
    void checkVersion(uint32 major, uint32 minor) const;

private:
    rapidjson::Document m_document;
    std::vector<char> m_buffer;

    std::vector<ImportMesh> m_meshes;

    std::string m_directory;
    std::string m_file;
};

} // namespace R3
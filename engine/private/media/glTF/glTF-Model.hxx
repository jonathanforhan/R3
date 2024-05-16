#pragma once

#include <fstream>
#include "api/Types.hpp"
#include "glTF.hxx"

namespace R3::glTF {

class Model : public Root {
public:
    explicit Model(const std::filesystem::path& path);

    [[nodiscard]] constexpr const std::vector<std::byte>& buffer() const { return m_buffer; }

private:
    bool parseGLB(std::ifstream& ifs);  // return true if success
    bool parseGLTF(std::ifstream& ifs); // return true if success

    void populateRoot();

    // top level populates
    void populateExtensionsUsed();
    void populateExtensionsRequired();
    void populateAccessors();
    void populateAnimations();
    void populateAsset();
    void populateBuffers();
    void populateBufferViews();
    void populateCameras();
    void populateImages();
    void populateMaterials();
    void populateMeshes();
    void populateNodes();
    void populateSamplers();
    void populateScene();
    void populateScenes();
    void populateSkins();
    void populateTextures();
    void populateExtensions();
    void populateExtras();

    void checkVersion(std::string_view version) const;
    void checkVersion(uint32 major, uint32 minor) const;

private:
    rapidjson::Document m_document;
    std::vector<std::byte> m_buffer;
    std::string m_path;
};

} // namespace R3::glTF

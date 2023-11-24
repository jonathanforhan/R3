#pragma once
#include <fstream>
#include <string_view>
#include <vector>
#include "api/Types.hpp"
#include "glTF.hxx"

namespace R3::glTF {

class Model : public Root {
public:
    explicit Model(std::string_view path);

    auto buffer() const -> const std::vector<char>& { return m_buffer; }

private:
    void parseGLB(std::ifstream& ifs);
    void parseGLTF(std::ifstream& ifs);

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

    // helper populates
    void populateTextureInfo(TextureInfo& textureInfo, rapidjson::Value& value);

    void checkVersion(std::string_view version) const;
    void checkVersion(uint32 major, uint32 minor) const;

private:
    rapidjson::Document m_document;
    std::vector<char> m_buffer;
    std::string m_path;
};

} // namespace R3::glTF
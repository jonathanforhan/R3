#pragma once

#include <filesystem>
#include <fstream>
#include <string>
#include <string_view>
#include <vector>
#include "api/Class.hpp"
#include "api/JSON.hpp"
#include "api/Types.hpp"
#include "glTF.hpp"

namespace R3::glTF {

class ModelImporter {
public:
    glTF::Root import(const std::filesystem::path& path);

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

    void checkVersion(std::string_view version) const;
    void checkVersion(uint32 major, uint32 minor) const;

    template <typename T>
    static void maybeAssign(T& dst, const json::Value& value, const char* key);

    static void maybeMove(json::Value& dst, json::Value& value, const char* key);

    static void populateTextureInfo(TextureInfo& textureInfo, json::Value& value);

private:
    glTF::Root* m_root = nullptr; // used by populate functions, denotes current root being populated in import()
    json::Document m_document;
    std::vector<uint8> m_buffer;
    std::string m_path;
};

} // namespace R3::glTF
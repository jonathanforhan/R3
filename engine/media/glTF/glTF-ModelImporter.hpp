#pragma once

#include <cstddef>
#include <filesystem>
#include <fstream>
#include <string_view>
#include <vector>
#include "engine/api/Class.hpp"
#include "engine/api/JSON.hpp"
#include "engine/api/Types.hpp"
#include "glTF.hpp"

namespace R3::glTF {

struct Model {
    R3_CTOR_DEFAULT(Model);
    R3_COPY_DELETE(Model);
    R3_MOVE_DEFAULT(Model);

    json::Document document;                 /// document is referenced by root's JSON values
    Root root;                               /// glTF root object
    std::vector<std::vector<std::byte>> bin; /// binary buffer chunks if present
};

class ModelImporter {
public:
    glTF::Model import(const std::filesystem::path& path);

private:
    void parseGLB(glTF::Model& model, std::ifstream& ifs);
    void parseGLTF(glTF::Model& model, std::ifstream& ifs);

    void populateModel(glTF::Model& model);

    // top level populates
    void populateExtensionsUsed(glTF::Model& model);
    void populateExtensionsRequired(glTF::Model& model);
    void populateAccessors(glTF::Model& model);
    void populateAnimations(glTF::Model& model);
    void populateAsset(glTF::Model& model);
    void populateBuffers(glTF::Model& model, std::vector<std::vector<std::byte>>& bin);
    void populateBufferViews(glTF::Model& model);
    void populateCameras(glTF::Model& model);
    void populateImages(glTF::Model& model);
    void populateMaterials(glTF::Model& model);
    void populateMeshes(glTF::Model& model);
    void populateNodes(glTF::Model& model);
    void populateSamplers(glTF::Model& model);
    void populateScene(glTF::Model& model);
    void populateScenes(glTF::Model& model);
    void populateSkins(glTF::Model& model);
    void populateTextures(glTF::Model& model);
    void populateExtensions(glTF::Model& model);
    void populateExtras(glTF::Model& model);

    void checkVersion(std::string_view version) const;
    void checkVersion(uint32 major, uint32 minor) const;

    template <typename T>
    void maybeAssign(T& dst, const json::Value& value, const char* key);

    void maybeMove(json::Value& dst, json::Value& value, const char* key);

    void populateTextureInfo(TextureInfo& textureInfo, json::Value& value);

private:
    std::filesystem::path m_path;
};

} // namespace R3::glTF
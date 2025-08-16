#pragma once

#include <filesystem>
#include <fstream>
#include <string>
#include <string_view>
#include <vector>
#include "JSON.hpp"
#include "Types.hpp"
#include "glTF.hpp"

namespace R3::glTF {

class Model : public Root {
public:
    explicit Model(const std::filesystem::path& path);

    ~Model() noexcept = default;

    Model(const Model&)            = delete;
    Model& operator=(const Model&) = delete;

    Model(Model&&) noexcept            = delete;
    Model& operator=(Model&&) noexcept = delete;

    [[nodiscard]] constexpr const std::vector<uint8>& buffer() const { return m_buffer; }

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

    template <typename T>
    static constexpr void maybeAssign(T& dst, const json::Value& value, const char* key);

    static void populateTextureInfo(TextureInfo& textureInfo, json::Value& value);

private:
    json::Document m_document;
    std::vector<uint8> m_buffer;
    std::string m_path;
};

} // namespace R3::glTF
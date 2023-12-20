#pragma once

#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>
#include "api/Types.hpp"
#include "core/BasicGeometry.hpp"
#include "render/Mesh.hpp"
#include "render/Shader.hpp"
#include "render/TextureBuffer.hpp"

namespace R3 {

namespace glTF {
struct Node;                 ///< @private
struct Mesh;                 ///< @private
struct Material;             ///< @private
struct TextureInfo;          ///< @private
struct NormalTextureInfo;    ///< @private
struct OcclusionTextureInfo; ///< @private
class Model;                 ///< @private
} // namespace glTF

class ModelComponent {
public:
    ModelComponent(const ModelComponent&) = delete;
    ModelComponent(ModelComponent&&) = default;

    /// @brief Construct a ModelComponent and add a ModelSystem to active scene if not present
    /// @param path path to glTF resourse
    /// @param shader shader used by model
    ModelComponent(const std::string& path);

    /// @brief Get the Model mesh list
    /// @return meshes
    std::vector<Mesh>& meshes() { return m_meshes; }

    /// @brief Get the texture data stored in ModelComponent
    /// @return textures
    std::vector<TextureBuffer>& textures() { return m_textures; }

private:
    void processNode(glTF::Model* model, glTF::Node* node);
    void processMesh(glTF::Model* model, glTF::Node* node, glTF::Mesh* mesh);
    void processMaterial(glTF::Model* model, glTF::Material* material);
    void processTexture(glTF::Model* model, glTF::TextureInfo* textureInfo, TextureType type);
    void processTexture(glTF::Model* model, glTF::NormalTextureInfo* textureInfo, TextureType type);
    void processTexture(glTF::Model* model, glTF::OcclusionTextureInfo* textureInfo, TextureType type);

private:
    std::vector<Mesh> m_meshes;
    std::vector<TextureBuffer> m_textures;
    std::unordered_map<uint32, usize> m_loadedTextures;
    std::string m_directory;
};

} // namespace R3
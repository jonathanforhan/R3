#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include "api/Types.hpp"
#include "render/Mesh.hpp"
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

struct MeshPrototype {
    std::vector<Vertex> vertices;
    std::vector<uint32> indices;
    std::vector<usize> textureIndices;
};

class ModelLoader {
public:
    ModelLoader() = default;
    ModelLoader(const ModelLoader&) = delete;
    ModelLoader(ModelLoader&&) = default;

    void load(const std::string& path, std::vector<Mesh>& meshes, std::vector<TextureBuffer>& textures);

private:
    void processNode(glTF::Model* model, glTF::Node* node);
    void processMesh(glTF::Model* model, glTF::Node* node, glTF::Mesh* mesh);
    void processMaterial(glTF::Model* model, glTF::Material* material);
    void processTexture(glTF::Model* model, glTF::TextureInfo* textureInfo, TextureType type);
    void processTexture(glTF::Model* model, glTF::NormalTextureInfo* textureInfo, TextureType type);
    void processTexture(glTF::Model* model, glTF::OcclusionTextureInfo* textureInfo, TextureType type);

private:
    std::vector<MeshPrototype> m_prototypes;
    std::vector<TextureBuffer> m_textures;
    std::unordered_map<uint32, usize> m_loadedTextures;
    std::string m_directory;
};

} // namespace R3
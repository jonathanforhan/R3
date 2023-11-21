#pragma once
#include <unordered_map>
#include <string_view>
#include <vector>
#include "api/Types.hpp"
#include "core/Mesh.hpp"
#include "core/Shader.hpp"
#include "core/Texture2D.hpp"

namespace R3 {

struct GLTF_Node;                 ///< @private
struct GLTF_Mesh;                 ///< @private
struct GLTF_Material;             ///< @private
struct GLTF_TextureInfo;          ///< @private
struct GLTF_NormalTextureInfo;    ///< @private
struct GLTF_OcclusionTextureInfo; ///< @private
class GLTF_Model;                 ///< @private

class ModelComponent {
public:
    ModelComponent(const std::string& path, Shader& shader);
    ModelComponent(const ModelComponent&) = delete;
    ModelComponent(ModelComponent&&) = default;
    void operator=(const ModelComponent&) = delete;
    ModelComponent& operator=(ModelComponent&&) = default;

    std::vector<Mesh>& meshes() { return m_meshes; }
    Shader& shader() { return m_shader; };
    std::vector<Texture2D>& textures() { return m_textures; }
    const std::string& path() const { return m_directory + m_file; }

public:
    mat4 transform{1.0f};
    float emissiveIntensity{1.0f};

private:
    void processNode(GLTF_Model* model, GLTF_Node* node);
    void processMesh(GLTF_Model* model, GLTF_Node* node, GLTF_Mesh* mesh);
    void processMaterial(GLTF_Model* model, GLTF_Material* material);
    void processTexture(GLTF_Model* model, GLTF_TextureInfo* textureInfo, TextureType type);
    void processTexture(GLTF_Model* model, GLTF_NormalTextureInfo* textureInfo, TextureType type);
    void processTexture(GLTF_Model* model, GLTF_OcclusionTextureInfo* textureInfo, TextureType type);

private:
    std::vector<Mesh> m_meshes;
    Shader& m_shader;
    std::vector<Texture2D> m_textures;
    std::unordered_map<uint32, usize> m_loadedTextures;
    std::string m_directory;
    std::string m_file;
};

} // namespace R3
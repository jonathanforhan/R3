#pragma once
#include <unordered_map>
#include <string_view>
#include <vector>
#include "api/Types.hpp"
#include "core/Mesh.hpp"
#include "core/Shader.hpp"
#include "core/Texture2D.hpp"

namespace R3 {

struct GLTF_Node;
struct GLTF_Mesh;
struct GLTF_Material;
struct GLTF_TextureInfo;
struct GLTF_NormalTextureInfo;
struct GLTF_OcclusionTextureInfo;
class GLTF_Model;

class ModelComponent {
public:
    ModelComponent(const std::string& path, Shader& shader);
    ModelComponent(const ModelComponent&) = delete;
    ModelComponent(ModelComponent&&) = default;
    void operator=(const ModelComponent&) = delete;
    ModelComponent& operator=(ModelComponent&&) = default;

    auto meshes() -> std::vector<Mesh>& { return m_meshes; }
    auto shader() -> Shader& { return m_shader; };
    auto textures() -> std::vector<Texture2D>& { return m_textures; }
    auto path() const -> const std::string& { return m_directory + m_file; }

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
    std::vector<Vertex> m_vertices;
    std::vector<uint32> m_indices;
    std::vector<Texture2D> m_textures;

    std::unordered_map<uint32, usize> m_loadedTextures;
    std::string m_directory;
    std::string m_file;
};

} // namespace R3
#pragma once
#include <set>
#include <string_view>
#include <vector>
#include "api/Types.hpp"
#include "core/Mesh.hpp"
#include "core/Shader.hpp"
#include "core/Texture2D.hpp"

class aiScene;
class aiNode;
class aiMesh;
class aiMaterial;

namespace R3 {

class ModelComponent {
public:
    ModelComponent(const std::string& directory, std::string_view file, Shader& shader, bool flipUVs = false);
    ModelComponent(ModelComponent&&) = default;

    auto mesh() -> Mesh& { return m_mesh; }
    auto shader() -> Shader& { return m_shader; };
    auto textures() -> std::vector<Texture2D>& { return m_textures; }

private:
    void processNode(aiNode* node, const aiScene* scene);
    void processMesh(aiMesh* mesh, const aiScene* scene);
    void loadMaterialTextures(aiMaterial* material, uint32 typeFlag, TextureType type);

public:
    mat4 transform{1.0f};
    vec2 tiling{1.0f};

private:
    Mesh m_mesh;
    Shader& m_shader;
    std::vector<Vertex> m_vertices;
    std::vector<uint32> m_indices;
    std::vector<Texture2D> m_textures;

    std::set<std::string> m_loadedTextures;
    std::string m_directory;
};

} // namespace R3
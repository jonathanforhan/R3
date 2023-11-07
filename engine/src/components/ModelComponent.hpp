#pragma once 
#include <string_view>
#include <vector>
#include "core/Mesh.hpp"
#include "core/Texture2D.hpp"
#include "core/Shader.hpp"

class aiScene;
class aiNode;
class aiMesh;
class aiMaterial;

namespace R3 {

class Model {
public:
    Model(std::string_view path, Shader& shader);

    auto meshes() -> std::vector<Mesh>& { return m_meshes; }
    auto textures() -> std::vector<Texture2D>& { return m_textures; }
    auto shader() -> Shader& { return m_shader; };

private:
    void processNode(aiNode* node, const aiScene* scene);
    void processMesh(aiMesh* mesh, const aiScene* scene);
    void loadMaterialTextures(aiMaterial* material, uint32 typeFlag, std::string_view typeName);

private:
    std::vector<Mesh> m_meshes;
    std::vector<Texture2D> m_textures;
    Shader& m_shader;
};

} // namespace R3
#pragma once
#include <string_view>
#include <unordered_map>
#include <vector>
#include "api/Types.hpp"
#include "core/BasicGeometry.hpp"
#include "core/Mesh.hpp"
#include "core/Texture2D.hpp"
#include "render/Shader.hpp"

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
    /// @brief Construct a ModelComponent and add a ModelSystem to active scene if not present
    /// @param path path to glTF resourse
    /// @param shader shader used by model
    ModelComponent(const std::string& path, Shader& shader);

    ModelComponent(const ModelComponent&) = delete;        ///< @brief non-copyable
    ModelComponent(ModelComponent&&) = default;            ///< @brief moveable
    void operator=(const ModelComponent&) = delete;        ///< @brief non-copyable
    ModelComponent& operator=(ModelComponent&&) = default; ///< @brief moveable

    /// @brief Get the Model mesh list
    /// @return meshes
    std::vector<Mesh>& meshes() { return m_meshes; }

    /// @brief Get the shader refernce used by ModelComponent
    /// @return shade ref
    Shader& shader() { return m_shader; };

    /// @brief Get the texture data stored in ModelComponent
    /// @return textures
    std::vector<Texture2D>& textures() { return m_textures; }

    /// @brief Get the glTF resource path used for the ModelComponent
    /// @return filepath
    const std::string& path() const { return m_directory + m_file; }

public:
    mat4 transform{1.0f};          ///< @brief ModelComponent Transform Matrix as used by shader
    float emissiveIntensity{1.0f}; ///< @brief Intensity of emissive material, ignored if no emissive texture present

private:
    void processNode(glTF::Model* model, glTF::Node* node);
    void processMesh(glTF::Model* model, glTF::Node* node, glTF::Mesh* mesh);
    void processMaterial(glTF::Model* model, glTF::Material* material);
    void processTexture(glTF::Model* model, glTF::TextureInfo* textureInfo, TextureType type);
    void processTexture(glTF::Model* model, glTF::NormalTextureInfo* textureInfo, TextureType type);
    void processTexture(glTF::Model* model, glTF::OcclusionTextureInfo* textureInfo, TextureType type);

private:
    std::vector<Mesh> m_meshes;
    Shader& m_shader;
    std::vector<Texture2D> m_textures;
    std::unordered_map<uint32, usize> m_loadedTextures;
    std::string m_directory;
    std::string m_file;
};

} // namespace R3
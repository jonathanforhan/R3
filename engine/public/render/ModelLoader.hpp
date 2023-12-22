#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include "api/Types.hpp"
#include "components/ModelComponent.hpp"
#include "render/TextureBuffer.hpp"
#include "render/Vertex.hpp"

#include "render/IndexBuffer.hpp"
#include "render/VertexBuffer.hpp"

#include "render/CommandPool.hpp"
#include "render/LogicalDevice.hpp"
#include "render/PhysicalDevice.hpp"
#include "render/Swapchain.hpp"

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
    VertexBuffer::ID vertexBuffer;
    IndexBuffer<uint32>::ID indexBuffer;
    std::vector<usize> textureIndices; // index of model-loader interal textures, not global pool
};

struct ModelLoaderSpecification {
    const PhysicalDevice& physicalDevice;
    const LogicalDevice& logicalDevice;
    const Swapchain& swapchain;
    const RenderPass& renderPass;
    const CommandPool& commandPool;
};

class ModelLoader {
public:
    ModelLoader(const ModelLoaderSpecification& spec)
        : m_spec(spec) {}
    ModelLoader(const ModelLoader&) = delete;
    ModelLoader(ModelLoader&&) = delete;

    void load(const std::string& path, ModelComponent& model);

private:
    void processNode(glTF::Model* model, glTF::Node* node);
    void processMesh(glTF::Model* model, glTF::Node* node, glTF::Mesh* mesh);
    void processMaterial(glTF::Model* model, glTF::Material* material);
    void processTexture(glTF::Model* model, glTF::TextureInfo* textureInfo, TextureType type);
    void processTexture(glTF::Model* model, glTF::NormalTextureInfo* textureInfo, TextureType type);
    void processTexture(glTF::Model* model, glTF::OcclusionTextureInfo* textureInfo, TextureType type);

private:
    ModelLoaderSpecification m_spec;
    std::vector<MeshPrototype> m_prototypes;
    std::vector<TextureBuffer::ID> m_textures;
    std::unordered_map<uint32, usize> m_loadedTextures;
    std::string m_directory;
};

} // namespace R3
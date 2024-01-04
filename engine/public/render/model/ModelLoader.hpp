#pragma once

/// Owned by Renderer and used to load in assets

#include <unordered_map>
#include "components/ModelComponent.hpp"
#include "render/IndexBuffer.hpp"
#include "render/RenderApi.hpp"
#include "render/TextureBuffer.hpp"
#include "render/VertexBuffer.hpp"

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

/// @brief Prototype used to build meshes
struct MeshPrototype {
    VertexBuffer::ID vertexBuffer;       ///< Allocated VertexBuffer
    IndexBuffer<uint32>::ID indexBuffer; ///< Allocated IndexBuffer
    std::vector<usize> textureIndices;   ///< index of model-loader interal textures, not global pool
};

/// @brief Model Loader Specification
struct ModelLoaderSpecification {
    const PhysicalDevice& physicalDevice;
    const LogicalDevice& logicalDevice;
    const Swapchain& swapchain;
    const RenderPass& renderPass;
    const CommandPool& commandPool;
    const StorageBuffer& storageBuffer; ///< Storage Buffer used for mouse picking
};

/// @brief ModelLoader used to load glTF Models
/// Owned by renderer and will allocate all the object needed by a ModelComponent
class ModelLoader {
public:
    DEFAULT_CONSTRUCT(ModelLoader);
    NO_COPY(ModelLoader);
    DEFAULT_MOVE(ModelLoader);

    /// @brief Construct ModelLoader from spec
    /// @param spec
    ModelLoader(const ModelLoaderSpecification& spec);

    /// @brief Load in a glTF Model from path
    /// @param path
    /// @param[out] model
    void load(const std::string& path, ModelComponent& model);

private:
    void processNode(glTF::Model* model, glTF::Node* node, uint32 nodeId);
    void processMesh(glTF::Model* model, glTF::Node* node, glTF::Mesh* mesh, uint32 nodeId);
    void processMaterial(glTF::Model* model, glTF::Material* material);
    void processTexture(glTF::Model* model, glTF::TextureInfo* textureInfo, TextureType type);
    void processTexture(glTF::Model* model, glTF::NormalTextureInfo* textureInfo, TextureType type);
    void processTexture(glTF::Model* model, glTF::OcclusionTextureInfo* textureInfo, TextureType type);
    void processKeyFrames(glTF::Model* model);

private:
    Ref<const PhysicalDevice> m_physicalDevice;
    Ref<const LogicalDevice> m_logicalDevice;
    Ref<const Swapchain> m_swapchain;
    Ref<const RenderPass> m_renderPass;
    Ref<const CommandPool> m_commandPool;
    Ref<const StorageBuffer> m_storageBuffer;

    std::vector<MeshPrototype> m_prototypes;
    std::vector<TextureBuffer::ID> m_textures;
    std::vector<KeyFrame> m_keyFrames;
    std::unordered_map<usize, usize> m_nodeIdToMeshIndex;
    std::unordered_map<uint32, usize> m_loadedTextures;
    std::string m_directory;
};

} // namespace R3
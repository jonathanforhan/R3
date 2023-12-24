#pragma once

/// @file ModelLoader.hpp
/// Owned by Renderer and used to load in assets

#include <string>
#include <unordered_map>
#include <vector>
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
    const PhysicalDevice& physicalDevice; ///< PhysicalDevice
    const LogicalDevice& logicalDevice;   ///< LogicalDevice
    const Swapchain& swapchain;           ///< Swapchain
    const RenderPass& renderPass;         ///< RenderPass
    const CommandPool& commandPool;       ///< CommandPool
};

/// @brief ModelLoader used to load glTF Models
/// Owned by renderer and will allocate all the object needed by a ModelComponent
class ModelLoader {
public:
    DEFAULT_CONSTRUCT(ModelLoader);
    NO_COPY(ModelLoader);
    DEFAULT_MOVE(ModelLoader);

    /// @brief Construct ModelLoader from spec
    /// Renderer is the only one who should have valid info for spec
    /// @param spec
    ModelLoader(const ModelLoaderSpecification& spec)
        : m_physicalDevice(&spec.physicalDevice),
          m_logicalDevice(&spec.logicalDevice),
          m_swapchain(&spec.swapchain),
          m_renderPass(&spec.renderPass),
          m_commandPool(&spec.commandPool) {}

    /// @brief Load in a glTF Model from path
    /// @param path
    /// @param[out] model
    void load(const std::string& path, ModelComponent& model);

private:
    void processNode(glTF::Model* model, glTF::Node* node);
    void processMesh(glTF::Model* model, glTF::Node* node, glTF::Mesh* mesh);
    void processMaterial(glTF::Model* model, glTF::Material* material);
    void processTexture(glTF::Model* model, glTF::TextureInfo* textureInfo, TextureType type);
    void processTexture(glTF::Model* model, glTF::NormalTextureInfo* textureInfo, TextureType type);
    void processTexture(glTF::Model* model, glTF::OcclusionTextureInfo* textureInfo, TextureType type);

private:
    Ref<const PhysicalDevice> m_physicalDevice;
    Ref<const LogicalDevice> m_logicalDevice;
    Ref<const Swapchain> m_swapchain;
    Ref<const RenderPass> m_renderPass;
    Ref<const CommandPool> m_commandPool;

    std::vector<MeshPrototype> m_prototypes;
    std::vector<TextureBuffer::ID> m_textures;
    std::unordered_map<uint32, usize> m_loadedTextures;
    std::string m_directory;
};

} // namespace R3
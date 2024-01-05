#pragma once

/// Owned by Renderer and used to load in assets

#include <unordered_map>
#include "components/ModelComponent.hpp"
#include "render/RenderApi.hpp"

namespace R3 {

namespace glTF {
struct Node;                 ///< @private
struct Mesh;                 ///< @private
struct Material;             ///< @private
struct TextureInfo;          ///< @private
struct NormalTextureInfo;    ///< @private
struct OcclusionTextureInfo; ///< @private
struct Skin;                 ///< @private
class Model;                 ///< @private
} // namespace glTF

struct MeshPrototype {
    id vertexBuffer = undefined;
    id indexBuffer = undefined;
    std::vector<id> textureIndices;
    mat4 transform;
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
    void processNode(glTF::Model& model, glTF::Node& node);
    void processMesh(glTF::Model& model, glTF::Node& node, glTF::Mesh& mesh);
    void processSkin(glTF::Model& model, glTF::Skin& skin);
    void processKeyFrames(glTF::Model& model);
    void processMaterial(glTF::Model& model, glTF::Material& material);
    void processTexture(glTF::Model& model, glTF::TextureInfo& textureInfo, TextureType type);
    void processTexture(glTF::Model& model, glTF::NormalTextureInfo& textureInfo, TextureType type);
    void processTexture(glTF::Model& model, glTF::OcclusionTextureInfo& textureInfo, TextureType type);

private:
    Ref<const PhysicalDevice> m_physicalDevice;
    Ref<const LogicalDevice> m_logicalDevice;
    Ref<const Swapchain> m_swapchain;
    Ref<const RenderPass> m_renderPass;
    Ref<const CommandPool> m_commandPool;
    Ref<const StorageBuffer> m_storageBuffer;

    std::vector<ModelNode> m_nodes;
    std::vector<MeshPrototype> m_prototypes;
    std::vector<KeyFrame> m_keyFrames;
    std::vector<Skin> m_skins;
    std::vector<id> m_textures;

    std::unordered_map<uint32, usize> m_loadedTextures;
    std::string m_directory;
};

} // namespace R3
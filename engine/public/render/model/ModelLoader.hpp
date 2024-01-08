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
    VertexBuffer vertexBuffer;
    IndexBuffer<uint32> indexBuffer;
    std::vector<usize> textureIndices;
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
    void load(const std::string& path,
              ModelComponent& model,
              const char* vertexShader = nullptr,
              const char* fragmentShader = nullptr);

private:
    void processNode(glTF::Model& model, glTF::Node& node);
    void processMesh(glTF::Model& model, glTF::Mesh& mesh);
    void processAnimations(glTF::Model& model);
    void processSkeleton(glTF::Model& model);
    void processJoint(glTF::Model& model, usize rootIndex, usize parentJoint);
    void processMaterial(glTF::Model& model, glTF::Material& material);
    void processTexture(glTF::Model& model, uint8 color[4], TextureType type);
    void processTexture(glTF::Model& model, glTF::TextureInfo& textureInfo, TextureType type);
    void processTexture(glTF::Model& model, glTF::NormalTextureInfo& textureInfo, TextureType type);
    void processTexture(glTF::Model& model, glTF::OcclusionTextureInfo& textureInfo, TextureType type);

    void preProcessTextures(glTF::Model& model);

private:
    Ref<const PhysicalDevice> m_physicalDevice;
    Ref<const LogicalDevice> m_logicalDevice;
    Ref<const Swapchain> m_swapchain;
    Ref<const RenderPass> m_renderPass;
    Ref<const CommandPool> m_commandPool;
    Ref<const StorageBuffer> m_storageBuffer;

    std::vector<MeshPrototype> m_prototypes;
    std::vector<KeyFrame> m_keyFrames;
    Skeleton m_skeleton;

    std::vector<std::shared_ptr<TextureBuffer>> m_textures;
    std::shared_ptr<TextureBuffer> m_nilTexture;
    std::string m_directory;
};

} // namespace R3
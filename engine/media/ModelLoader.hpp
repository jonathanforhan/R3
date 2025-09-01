#pragma once

#include <filesystem>
#include <future>
#include <map>
#include <shared_mutex>
#include <string>
#include <string_view>
#include <vector>
#include "ImageLoader.hpp"
#include "engine/api/Api.hpp"
#include "engine/api/Class.hpp"
#include "engine/api/Types.hpp"
#include "engine/core/Entity.hpp"
#include "engine/render/Flags.hpp"
#include "engine/render/ShaderObjects.hpp"
#include "engine/render/vulkan/vulkan-CommandBuffer.hpp"
#include "glTF/glTF-ModelImporter.hpp"
#include "glTF/glTF.hpp"

namespace R3 {

class R3_API ModelLoader {
public:
    R3_CTOR_DEFAULT(ModelLoader);
    R3_COPY_DELETE(ModelLoader);
    R3_MOVE_DEFAULT(ModelLoader);

    Entity glTFLoad(const std::filesystem::path& path);

    std::future<Entity> glTFLoadAsync(const std::filesystem::path& path);

private:
    void glTF_processNode(Entity entity, const glTF::Model& model, const glTF::Node& node);
    void glTF_processNodeTransform(Entity entity, const glTF::Model& model, const glTF::Node& node);
    void glTF_processMesh(Entity entity, const glTF::Model& model, const glTF::Mesh& mesh);
    void glTF_processMeshPrimitive(Entity entity, const glTF::Model& model, const glTF::MeshPrimitive& primitive);
    void glTF_processVertices(Entity entity, const glTF::Model& model, const std::map<std::string, uint32>& attributes);
    void glTF_processIndices(Entity entity, const glTF::Model& model, uint32 indices);
    void glTF_processAnimations(Entity entity, const glTF::Model& model);
    void glTF_processSkeleton(Entity entity, const glTF::Model& model);
    void glTF_processJoint(Entity entity, const glTF::Model& model, usize rootIndex, usize parentJoint);
    void glTF_processMaterial(Entity entity, const glTF::Model& model, const glTF::Material& material);
    void glTF_processTexture(Entity entity, const glTF::Model& model, const glTF::Texture& texture, TextureType type);
    void glTF_processTexture(Entity entity, const glTF::Model& model, uint8 color[4], TextureType type);
    void glTF_processTextureInfo(Entity entity,
                                 const glTF::Model& model,
                                 const glTF::TextureInfo& textureInfo,
                                 TextureType type);
    void glTF_processTextureInfo(Entity entity,
                                 const glTF::Model& model,
                                 const glTF::NormalTextureInfo& textureInfo,
                                 TextureType type);
    void glTF_processTextureInfo(Entity entity,
                                 const glTF::Model& model,
                                 const glTF::OcclusionTextureInfo& textureInfo,
                                 TextureType type);
    void glTF_preprocessImageFiles(const glTF::Model& model);
    // can take a vector of <typename T> and read the accessor as <typename U>
    // useful for converting datatypes without multiple vector copies
    // eg accesor<uint16>[] -> std::vector<uint32>
    template <typename T, typename U = T>
    void glTF_readAccessor(const glTF::Model& model, uint32 iAccessor, std::vector<T>& out);
    usize glTF_sizeof(uint32 datatype); /// get size in bytes of glTF accessor datatype
    usize glTF_componentElements(std::string_view componentType);

    std::string glTF_textureKey(std::filesystem::path path, TextureType type);
    std::string glTF_textureKey(uint32 textureSource, uint32 bufferView, TextureType type);
    std::string glTF_imageKey(std::filesystem::path path);
    std::string glTF_embeddedImageKey(uint32 textureSource, uint32 bufferView);
    std::string glTF_colorKey(uint8 color[4]);
    std::string glTF_vertexBufferKey(uint32 index);
    std::string glTF_indexBufferKey(uint32 index);

    // const to make sure no (undesired) mutation
    const std::filesystem::path& path() const noexcept { return m_path; };

private:
    std::filesystem::path m_path;
    vulkan::CommandBuffer* m_cmd = nullptr;
    std::map<std::string, ImageLoader::ImageDescriptor> m_cachedImages;
    std::vector<Vertex> m_cachedVertices;
    std::vector<uint32> m_cachedIndices;

    static std::shared_mutex s_modelLoaderLock;
};

} // namespace R3

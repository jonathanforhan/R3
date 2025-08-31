#pragma once

#include "engine/api/Api.hpp"
#include <filesystem>
#include <string_view>
#include <vector>
#include "engine/api/Types.hpp"
#include "engine/core/Entity.hpp"
#include "glTF/glTF-ModelImporter.hpp"
#include "glTF/glTF.hpp"
#include "engine/render/Flags.hpp"
#include "engine/render/vulkan/vulkan-CommandBuffer.hpp"

namespace R3 {

class R3_API ModelLoader {
public:
    Entity glTFLoad(const std::filesystem::path& path);

private:
    void glTF_processNode(Entity entity, glTF::Model& model, glTF::Node& node);
    void glTF_processMesh(Entity entity, glTF::Model& model, glTF::Mesh& mesh);
    void glTF_processAnimations(Entity entity, glTF::Model& model);
    void glTF_processSkeleton(Entity entity, glTF::Model& model);
    void glTF_processJoint(Entity entity, glTF::Model& model, usize rootIndex, usize parentJoint);
    void glTF_processMaterial(Entity entity, glTF::Model& model, glTF::Material& material);
    void glTF_processTexture(Entity entity, glTF::Model& model, glTF::Texture& texture, TextureType type);
    void glTF_processTexture(Entity entity, glTF::Model& model, uint8 color[4], TextureType type);
    void glTF_processTextureInfo(Entity entity, glTF::Model& model, glTF::TextureInfo& textureInfo, TextureType type);
    void glTF_processTextureInfo(Entity entity,
                                 glTF::Model& model,
                                 glTF::NormalTextureInfo& textureInfo,
                                 TextureType type);
    void glTF_processTextureInfo(Entity entity,
                                 glTF::Model& model,
                                 glTF::OcclusionTextureInfo& textureInfo,
                                 TextureType type);
    // can take a vector of <typename T> and read the accessor as <typename U>
    // useful for converting datatypes without multiple vector copies
    // eg accesor<uint16>[] -> std::vector<uint32>
    template <typename T, typename U = T>
    void glTF_readAccessor(glTF::Model& model, usize iAccessor, std::vector<T>& out);
    usize glTF_sizeof(uint32 datatype); /// get size in bytes of glTF accessor datatype
    usize glTF_componentElements(std::string_view componentType);

private:
    std::filesystem::path m_path;
    vulkan::CommandBuffer* m_cmd;
};

} // namespace R3

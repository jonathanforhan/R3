#pragma once

#include <filesystem>
#include <string_view>
#include <vector>
#include "api/Types.hpp"
#include "core/Entity.hpp"
#include "glTF/glTF-ModelImporter.hpp"
#include "glTF/glTF.hpp"
#include "render/Flags.hpp"
#include "render/vulkan/vulkan-CommandBuffer.hpp"

namespace R3 {

class ModelLoader {
public:
    Entity glTFLoad(const std::filesystem::path& path);

private:
    void glTF_processNode(glTF::Model& model, glTF::Node& node, usize id);
    void glTF_processMesh(glTF::Model& model, glTF::Mesh& mesh, usize id);
    void glTF_processAnimations(glTF::Model& model, usize id);
    void glTF_processSkeleton(glTF::Model& model, usize id);
    void glTF_processJoint(glTF::Model& model, usize rootIndex, usize parentJoint, usize id);
    void glTF_processMaterial(glTF::Model& model, glTF::Material& material, usize id);
    void glTF_processTexture(glTF::Model& model, glTF::Texture& texture, TextureType type, usize id);
    void glTF_processTexture(glTF::Model& model, uint8 color[4], TextureType type, usize id);
    void glTF_processTextureInfo(glTF::Model& model, glTF::TextureInfo& textureInfo, TextureType type, usize id);
    void glTF_processTextureInfo(glTF::Model& model, glTF::NormalTextureInfo& textureInfo, TextureType type, usize id);
    void glTF_processTextureInfo(glTF::Model& model,
                                 glTF::OcclusionTextureInfo& textureInfo,
                                 TextureType type,
                                 usize id);
    // can take a vector of <typename T> and read the accessor as <typename U>
    // useful for converting datatypes without multiple vector copies
    // eg accesor<uint16>[] -> std::vector<uint32>
    template <typename T, typename U = T>
    void glTF_readAccessor(glTF::Model& model, usize iAccessor, std::vector<T>& out);
    usize glTF_sizeof(uint32 datatype); /// get size in bytes of glTF accessor datatype
    usize glTF_componentElements(std::string_view componentType);

private:
    Entity m_entity = entt::null;
    std::filesystem::path m_path;
    vulkan::CommandBuffer* m_cmd;
};

} // namespace R3

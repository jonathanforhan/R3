#pragma once

#include <filesystem>
#include <string_view>
#include <vector>
#include "api/Types.hpp"
#include "core/Entity.hpp"
#include "glTF/glTF-ModelImporter.hpp"
#include "glTF/glTF.hpp"
#include "render/Flags.hpp"
#include "render/vulkan/vulkan-Buffer.hpp"
#include "render/vulkan/vulkan-Image.hpp"
#include "render/vulkan/vulkan-Texture.hpp"

namespace R3 {

class ModelLoader {
public:
    Entity glTFLoad(const std::filesystem::path& path);

private:
    void glTF_processNode(glTF::Model& model, glTF::Node& node);
    void glTF_processMesh(glTF::Model& model, glTF::Mesh& mesh);
    void glTF_processAnimations(glTF::Model& model);
    void glTF_processSkeleton(glTF::Model& model);
    void glTF_processJoint(glTF::Model& model, usize rootIndex, usize parentJoint);
    void glTF_processMaterial(glTF::Model& model, glTF::Material& material);
    void glTF_processTexture(glTF::Model& model, uint8 color[4], TextureType type);
    void glTF_processTexture(glTF::Model& model, glTF::TextureInfo& textureInfo, TextureType type);
    void glTF_processTexture(glTF::Model& model, glTF::NormalTextureInfo& textureInfo, TextureType type);
    void glTF_processTexture(glTF::Model& model, glTF::OcclusionTextureInfo& textureInfo, TextureType type);
    void glTF_preProcessTextures(glTF::Model& model);
    // can take a vector of <typename T> and read the accessor as <typename U>
    // useful for converting datatypes without multiple vector copies
    // eg accesor<uint16>[] -> std::vector<uint32>
    template <typename T, typename U = T>
    void glTF_readAccessor(glTF::Model& model, usize iAccessor, std::vector<T>& out);
    usize glTF_sizeof(uint32 datatype); /// get size in bytes of glTF accessor datatype
    usize glTF_componentElements(std::string_view componentType);

private:
    Entity m_entity = entt::null;
    std::filesystem::path m_directory;
    // std::vector<vulkan::Buffer> m_buffers;
    // std::vector<vulkan::Image> m_images;
    // std::vector<vulkan::Texture> m_textures;
};

} // namespace R3

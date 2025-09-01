#include "ModelLoader.hpp"

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <execution>
#include <filesystem>
#include <format>
#include <future>
#include <map>
#include <memory>
#include <mutex>
#include <optional>
#include <shared_mutex>
#include <span>
#include <string>
#include <string_view>
#include <utility>
#include <vector>
#include <vulkan/vulkan.h>
#include <entt/resource/resource.hpp>
#include "ImageLoader.hpp"
#include "api/Assert.hpp"
#include "api/Exception.hpp"
#include "api/Types.hpp"
#include "components/HierarchyComponent.hpp"
#include "components/MaterialComponent.hpp"
#include "components/MeshComponent.hpp"
#include "components/TextureLifetimeComponent.hpp"
#include "components/TransformComponent.hpp"
#include "core/Engine.hpp"
#include "core/Entity.hpp"
#include "core/Log.hpp"
#include "core/ResourceManager.hpp"
#include "core/World.hpp"
#include "glTF/glTF-ModelImporter.hpp"
#include "glTF/glTF.hpp"
#include "render/Flags.hpp"
#include "render/ShaderObjects.hpp"
#include "render/vulkan/vulkan-Buffer.hpp"
#include "render/vulkan/vulkan-CommandBuffer.hpp"
#include "render/vulkan/vulkan-RenderContext.hpp"
#include "render/vulkan/vulkan-Texture.hpp"

namespace R3 {

std::shared_mutex ModelLoader::s_modelLoaderLock;

Entity ModelLoader::glTFLoad(const std::filesystem::path& path) {
    m_path = path;

    glTF::Model model = glTF::ModelImporter().import(path);
    glTF_preProcessImageFiles(model);

    std::unique_lock lock(s_modelLoaderLock);
    {
        LOG_INFO("Loading model: {}", path.string());

        Entity root = GWorld()->registry().create();
        GWorld()->registry().emplace<TransformComponent>(root); // give root node a default transform

        vulkan::RenderContext& ctx = GEngine()->RenderContext<vulkan::RenderContext>();

        m_cmd = &ctx.graphicsCommandBuffer(0);
        m_cmd->begin();

        for (glTF::Scene& scene : model.root.scenes) {
            for (uint32 iNode : scene.nodes) {
                glTF_processNode(root, model, model.root.nodes[iNode]);
            }
        }

        m_cmd->end();
        ctx.submitSync(ctx.graphicsQueue(), m_cmd->commandBuffer());

        // the device resources are in ResourceManager cache so we can clear our temp caches
        m_cachedImages.clear();
        m_cachedVertices.clear();
        m_cachedIndices.clear();

        return root;
    }
}

std::future<Entity> ModelLoader::glTFLoadAsync(const std::filesystem::path& path) {
    return std::async(std::launch::async, [=] {
        ModelLoader loader;
        return loader.glTFLoad(path);
    });
}

void ModelLoader::glTF_processNode(Entity entity, const glTF::Model& model, const glTF::Node& node) {
    Entity parent = entity;

    Entity child{GWorld()->registry().create()};
    GWorld()->registry().get_or_emplace<HierarchyComponent>(parent).children.push_back(child); // add child to parent

    fmat4 local;
    for (usize i = 0; i < 16; i++) {
        local[i / 4][i % 4] = node.matrix[i];
    }

    fquat rotation    = glm::normalize(fquat{node.rotation[3], node.rotation[0], node.rotation[1], node.rotation[2]});
    fvec3 scale       = fvec3{node.scale[0], node.scale[1], node.scale[2]};
    fvec3 translation = fvec3{node.translation[0], node.translation[1], node.translation[2]};

    fmat4 T = glm::translate(fmat4(1.0f), translation);
    fmat4 R = glm::mat4_cast(rotation);
    fmat4 S = glm::scale(fmat4(1.0f), scale);
    local *= T * R * S;

    GWorld()->registry().emplace<TransformComponent>(child).transform() = local;

    if (node.mesh) {
        if (model.root.meshes[*node.mesh].primitives.size() > 1) {
            // this node is a parent for multiple mesh primitives, give it a hierarchy component
            GWorld()->registry().get_or_emplace<HierarchyComponent>(child).parent = parent;
        }
        glTF_processMesh(child, model, model.root.meshes[*node.mesh]);
    }

    if (!node.children.empty()) {
        // set parent of this node
        GWorld()->registry().get_or_emplace<HierarchyComponent>(child).parent = parent;

        for (uint32 iChild : node.children) {
            glTF_processNode(child, model, model.root.nodes[iChild]);
        }
    }
}

void ModelLoader::glTF_processMesh(Entity entity, const glTF::Model& model, const glTF::Mesh& mesh) {
    bool isParent = mesh.primitives.size() > 1;

    for (const glTF::MeshPrimitive& primitive : mesh.primitives) {
        Entity currentEntity;

        if (!isParent) {
            currentEntity = entity;
        } else {
            Entity child = GWorld()->registry().create();
            GWorld()->registry().emplace<TransformComponent>(child);
            GWorld()->registry().get<HierarchyComponent>(entity).children.push_back(child);
            currentEntity = child;
        }

        R3_ASSERT(primitive.mode == glTF::TRIANGLES, "Only TRIANGLES mode is supported");
        glTF_processVertices(currentEntity, model, primitive.attributes);

        if (primitive.indices) {
            glTF_processIndices(currentEntity, model, *primitive.indices);
        }

        if (primitive.material) {
            glTF_processMaterial(currentEntity, model, model.root.materials[*primitive.material]);
        }
    }
}

void ModelLoader::glTF_processVertices(Entity entity,
                                       const glTF::Model& model,
                                       const std::map<std::string, uint32>& attributes) {
    R3_ASSERT(attributes.contains(glTF::POSITION));

    std::string key = glTF_vertexBufferKey(attributes.at(glTF::POSITION));
    LOG_INFO("importing vertices {}", key);

    usize vertexCount = model.root.accessors[attributes.at(glTF::POSITION)].count;

    auto&& [vbo, vboLoaded] = GResourceManager()->loadBuffer(
        std::string_view(key), nullptr, vertexCount * sizeof(Vertex), BufferPreset::DeviceVertex);

    if (vboLoaded) {
        std::vector<fvec3> positions;
        glTF_readAccessor(model, attributes.at(glTF::POSITION), positions);

        std::vector<fvec3> normals;
        if (attributes.contains(glTF::NORMAL)) {
            glTF_readAccessor(model, attributes.at(glTF::NORMAL), normals);
        }

        std::vector<fvec2> texCoords;
        if (attributes.contains(glTF::TEXCOORD_0)) {
            glTF_readAccessor(model, attributes.at(glTF::TEXCOORD_0), texCoords);
        }

        std::vector<ivec4> joints;
        if (attributes.contains(glTF::JOINTS_0)) {
            usize index = attributes.at(glTF::JOINTS_0);
            if (glTF_sizeof(model.root.accessors[index].componentType) == sizeof(uint8)) {
                glTF_readAccessor<ivec4, u8vec4>(model, index, joints);
            } else if (glTF_sizeof(model.root.accessors[index].componentType) == sizeof(uint16)) {
                glTF_readAccessor<ivec4, u16vec4>(model, index, joints);
            } else {
                throw Exception{
                    std::format("unsupported joint datatype {}", model.root.accessors[index].componentType)};
            }
        }

        std::vector<fvec4> weights;
        if (attributes.contains(glTF::WEIGHTS_0)) {
            usize index = attributes.at(glTF::WEIGHTS_0);

            if (glTF_sizeof(model.root.accessors[index].componentType) == sizeof(uint8)) {
                glTF_readAccessor<fvec4, u8vec4>(model, index, weights);
            } else if (glTF_sizeof(model.root.accessors[index].componentType) == sizeof(uint16)) {
                glTF_readAccessor<fvec4, u16vec4>(model, index, weights);
            } else {
                glTF_readAccessor(model, index, weights);
            }
        }

        vulkan::Buffer* vertexStagingBuffer = GResourceManager()->newFrameScopedObject<vulkan::Buffer>();
        VkBufferCopy2* vertexCopyRegion     = GResourceManager()->newFrameScopedObject<VkBufferCopy2>();

        *vertexStagingBuffer = vulkan::Buffer{nullptr, vertexCount * sizeof(Vertex), BufferPreset::Staging};
        for (usize i = 0; i < vertexCount; i++) {
            const Vertex vertex = {
                .position      = positions[i],
                .normal        = i < normals.size() ? normals[i] : fvec3(0.0f),
                .textureCoords = i < texCoords.size() ? texCoords[i] : fvec2(0.0f),
                .boneIDs       = i < joints.size() ? joints[i] : ivec4(-1),
                .weights       = i < weights.size() ? weights[i] : fvec4(0.0f),
            };
            vertexStagingBuffer->copy(&vertex, sizeof(Vertex), i * sizeof(Vertex));
        }
        *vertexCopyRegion = {
            .sType     = VK_STRUCTURE_TYPE_BUFFER_COPY_2,
            .srcOffset = 0,
            .dstOffset = 0,
            .size      = vertexCount * sizeof(Vertex),
        };
        m_cmd->copyBuffer({
            .sType       = VK_STRUCTURE_TYPE_COPY_BUFFER_INFO_2,
            .srcBuffer   = vertexStagingBuffer->buffer(),
            .dstBuffer   = vbo->buffer(),
            .regionCount = 1,
            .pRegions    = vertexCopyRegion,
        });
    }

    R3_ASSERT(!GWorld()->registry().try_get<MeshComponent>(entity));

    auto& mesh             = GWorld()->registry().emplace<MeshComponent>(entity);
    mesh.vertexBufferIndex = std::move(vbo);
    mesh.vertexCount       = vertexCount;
}

void ModelLoader::glTF_processIndices(Entity entity, const glTF::Model& model, uint32 iIndices) {
    std::string key = glTF_indexBufferKey(iIndices);
    LOG_INFO("importing indices {}", key);

    const glTF::Accessor& accessor = model.root.accessors[iIndices];

    usize indexCount = accessor.count;

    auto&& [ibo, iboLoaded] = GResourceManager()->loadBuffer(
        std::string_view(key), nullptr, indexCount * sizeof(uint32), BufferPreset::DeviceIndex);

    std::vector<uint32> indices;
    if (iboLoaded) {
        switch (glTF_sizeof(accessor.componentType)) {
            case sizeof(uint8):
                glTF_readAccessor<uint32, uint8>(model, iIndices, indices);
                break;
            case sizeof(uint16):
                glTF_readAccessor<uint32, uint16>(model, iIndices, indices);
                break;
            case sizeof(uint32):
                glTF_readAccessor(model, iIndices, indices);
                break;
            default:
                throw Exception{std::format("unsupported index datatype {}", accessor.componentType)};
        }

        vulkan::Buffer* indexStagingBuffer = GResourceManager()->newFrameScopedObject<vulkan::Buffer>();
        VkBufferCopy2* indexCopyRegion     = GResourceManager()->newFrameScopedObject<VkBufferCopy2>();

        *indexStagingBuffer = vulkan::Buffer{std::span<const uint32>{indices}, BufferPreset::Staging};
        *indexCopyRegion    = {
               .sType     = VK_STRUCTURE_TYPE_BUFFER_COPY_2,
               .srcOffset = 0,
               .dstOffset = 0,
               .size      = indices.size() * sizeof(uint32),
        };
        m_cmd->copyBuffer({
            .sType       = VK_STRUCTURE_TYPE_COPY_BUFFER_INFO_2,
            .srcBuffer   = indexStagingBuffer->buffer(),
            .dstBuffer   = ibo->buffer(),
            .regionCount = 1,
            .pRegions    = indexCopyRegion,
        });
    }

    R3_ASSERT(GWorld()->registry().try_get<MeshComponent>(entity));

    auto& mesh            = GWorld()->registry().get<MeshComponent>(entity);
    mesh.indexBufferIndex = std::move(ibo);
    mesh.indexCount       = indexCount;
}

void ModelLoader::glTF_processAnimations(Entity entity, const glTF::Model& model) {
    /* TODO */
}

void ModelLoader::glTF_processSkeleton(Entity entity, const glTF::Model& model) {
    /* TODO */
}

void ModelLoader::glTF_processJoint(Entity entity, const glTF::Model& model, usize modelIndex, usize parentJoint) {
    /* TODO */
}

void ModelLoader::glTF_processMaterial(Entity entity, const glTF::Model& model, const glTF::Material& material) {
    if (material.emissiveTexture) {
        glTF_processTextureInfo(entity, model, *material.emissiveTexture, TextureType::Emissive);
    }

    if (material.occlusionTexture) {
        glTF_processTextureInfo(entity, model, *material.occlusionTexture, TextureType::AmbientOcclusion);
    }

    if (material.normalTexture) {
        glTF_processTextureInfo(entity, model, *material.normalTexture, TextureType::Normal);
    }

    if (material.pbrMetallicRoughness->metallicRoughnessTexture) {
        glTF_processTextureInfo(
            entity, model, *material.pbrMetallicRoughness->metallicRoughnessTexture, TextureType::MetallicRoughness);
    }

    if (material.pbrMetallicRoughness->baseColorTexture) {
        glTF_processTextureInfo(entity, model, *material.pbrMetallicRoughness->baseColorTexture, TextureType::Albedo);
    } else {
        uint8 color[4] = {
            static_cast<uint8>(material.pbrMetallicRoughness->baseColorFactor[0] * 255.0f),
            static_cast<uint8>(material.pbrMetallicRoughness->baseColorFactor[1] * 255.0f),
            static_cast<uint8>(material.pbrMetallicRoughness->baseColorFactor[2] * 255.0f),
            static_cast<uint8>(material.pbrMetallicRoughness->baseColorFactor[3] * 255.0f),
        };
        glTF_processTexture(entity, model, color, TextureType::Albedo);
    }
}

void ModelLoader::glTF_processTexture(Entity entity,
                                      const glTF::Model& model,
                                      const glTF::Texture& texture,
                                      TextureType type) {
    if (!texture.source) {
        return;
    }

    const glTF::Image& image = model.root.images[*texture.source];

    Handle<vulkan::Texture> hTexture;
    std::string key;

    if (!image.uri.empty()) {
        std::filesystem::path imagePath = path();
        imagePath.replace_filename(image.uri);

        key = glTF_imageKey(imagePath);
        LOG_INFO("importing texture {}", key);

        auto&& [tex, texLoaded] = GResourceManager()->loadTexture(std::string_view(key));

        if (texLoaded) {
            vulkan::Buffer* stagingBuffer = GResourceManager()->newFrameScopedObject<vulkan::Buffer>();

            R3_ASSERT(m_cachedImages.contains(key));
            auto& imgDesc = m_cachedImages.at(key);

            *tex = vulkan::Texture{
                *m_cmd, imgDesc.data.get(), imgDesc.width, imgDesc.height, imgDesc.channels, type, *stagingBuffer};
        }

        hTexture = std::move(tex);
    } else {
        const glTF::BufferView& bufferView = model.root.bufferViews[*image.bufferView];
        const std::byte* data              = &(model.bin[bufferView.buffer][bufferView.byteOffset]);

        key = glTF_embeddedImageKey(*texture.source, *image.bufferView);
        LOG_INFO("importing texture {}", key);
        auto&& [tex, texLoaded] = GResourceManager()->loadTexture(std::string_view(key));

        if (texLoaded) {
            vulkan::Buffer* stagingBuffer = GResourceManager()->newFrameScopedObject<vulkan::Buffer>();

            R3_ASSERT(m_cachedImages.contains(key));
            auto& imgDesc = m_cachedImages.at(key);

            *tex = vulkan::Texture{
                *m_cmd, imgDesc.data.get(), imgDesc.width, imgDesc.height, imgDesc.channels, type, *stagingBuffer};
        }

        hTexture = std::move(tex);
    }

    MaterialComponent& mat = GWorld()->registry().get_or_emplace<MaterialComponent>(entity);
    uint32 slot            = GResourceManager()->bindTexture(std::string_view(key), *hTexture);
    mat.setTextureSlot(type, slot);

    TextureLifetimeComponent& textureLifetime = GWorld()->registry().get_or_emplace<TextureLifetimeComponent>(entity);
    textureLifetime.textures.emplace_back(std::move(hTexture)); // ensure texture lives as long as entity
}

void ModelLoader::glTF_processTexture(Entity entity, const glTF::Model& model, uint8 color[4], TextureType type) {
    std::string key = glTF_colorKey(color);

    LOG_INFO("importing texture {}", key);
    auto&& [tex, texLoaded] = GResourceManager()->loadTexture(std::string_view(key));

    if (texLoaded) {
        vulkan::Buffer* stagingBuffer = GResourceManager()->newFrameScopedObject<vulkan::Buffer>();

        *tex = vulkan::Texture{*m_cmd, (const std::byte*)color, 1, 1, 4, type, *stagingBuffer};
    }

    MaterialComponent& mat = GWorld()->registry().get_or_emplace<MaterialComponent>(entity);
    uint32 slot            = GResourceManager()->bindTexture(std::string_view(key), *tex);
    mat.setTextureSlot(type, slot);

    TextureLifetimeComponent& textureLifetime = GWorld()->registry().get_or_emplace<TextureLifetimeComponent>(entity);
    textureLifetime.textures.emplace_back(std::move(tex)); // ensure texture lives as long as entity
}

void ModelLoader::glTF_processTextureInfo(Entity entity,
                                          const glTF::Model& model,
                                          const glTF::TextureInfo& textureInfo,
                                          TextureType type) {
    const glTF::Texture& texture = model.root.textures[textureInfo.index];
    glTF_processTexture(entity, model, texture, type);
}

void ModelLoader::glTF_processTextureInfo(Entity entity,
                                          const glTF::Model& model,
                                          const glTF::NormalTextureInfo& textureInfo,
                                          TextureType type) {
    glTF::TextureInfo adapter{.index = textureInfo.index};
    glTF_processTextureInfo(entity, model, adapter, type);
}

void ModelLoader::glTF_processTextureInfo(Entity entity,
                                          const glTF::Model& model,
                                          const glTF::OcclusionTextureInfo& textureInfo,
                                          TextureType type) {
    glTF::TextureInfo adapter{.index = textureInfo.index};
    glTF_processTextureInfo(entity, model, adapter, type);
}

void ModelLoader::glTF_preProcessImageFiles(const glTF::Model& model) {
    std::vector<uint32> textureSources;
    for (const glTF::Texture& texture : model.root.textures) {
        if (!texture.source) {
            continue;
        }

        uint32 source = *texture.source;

        if (std::find(textureSources.begin(), textureSources.end(), source) != textureSources.end()) {
            continue; // already queued
        }

        textureSources.push_back(source);
    }

    std::vector<std::pair<std::string, ImageLoader::ImageDescriptor>> imageKeyValues(textureSources.size());

    auto parLoadImages = [&](uint32 textureSource) -> std::pair<std::string, ImageLoader::ImageDescriptor> {
        const glTF::Image& image = model.root.images[textureSource];

        std::string imageKey;
        ImageLoader::ImageDescriptor imageDesc;

        if (image.uri.empty()) {
            R3_ASSERT(image.bufferView.has_value());
            const glTF::BufferView& bufferView = model.root.bufferViews[*image.bufferView];
            const std::byte* data              = &(model.bin[bufferView.buffer][bufferView.byteOffset]);

            imageKey  = glTF_embeddedImageKey(textureSource, *image.bufferView);
            imageDesc = ImageLoader::loadImageCompressed(data, bufferView.byteLength);
        } else {
            std::filesystem::path imagePath = path();
            imagePath.replace_filename(image.uri);

            imageKey  = glTF_imageKey(imagePath);
            imageDesc = ImageLoader::loadImageFile(imagePath);
        }

        return std::make_pair(std::move(imageKey), std::move(imageDesc));
    };
    std::transform(
        std::execution::par_unseq, textureSources.begin(), textureSources.end(), imageKeyValues.begin(), parLoadImages);

    for (auto& [imageKey, imageValue] : imageKeyValues) {
        if (imageValue.data == nullptr) {
            LOG_WARNING("Failed to load image: {}", imageKey);
            continue;
        }
        R3_ASSERT(imageValue.data.get());
        m_cachedImages.emplace(std::move(imageKey), std::move(imageValue));
    }
}

template <typename T, typename U>
void ModelLoader::glTF_readAccessor(const glTF::Model& model, usize iAccessor, std::vector<T>& out) {
    const glTF::Accessor& accessor = model.root.accessors[iAccessor];
    R3_ASSERT(glTF_sizeof(accessor.componentType) * glTF_componentElements(accessor.type) == sizeof(U));

    if (!accessor.bufferView) {
        throw Exception{"Sparse accessors are not supported"};
    }

    const glTF::BufferView& bufferView = model.root.bufferViews[*accessor.bufferView];

    const uint32 offset = accessor.byteOffset + bufferView.byteOffset;
    const uint32 stride = bufferView.byteStride ? *bufferView.byteStride : sizeof(U);

    out.reserve(out.size() + accessor.count);
    for (usize i = 0; i < accessor.count; i++) {
        // read as U but cast to T
        out.emplace_back(
            static_cast<T>(*reinterpret_cast<const U*>(&model.bin[bufferView.buffer][offset + (i * stride)])));
    }
}

usize ModelLoader::glTF_sizeof(uint32 datatype) {
    switch (datatype) {
        case glTF::UNSIGNED_BYTE:
        case glTF::BYTE:
            return sizeof(uint8);
        case glTF::UNSIGNED_SHORT:
        case glTF::SHORT:
            return sizeof(uint16);
        case glTF::UNSIGNED_INT:
        case glTF::INT:
            return sizeof(uint32);
        case glTF::FLOAT:
            return sizeof(float);
        default:
            throw Exception{std::format("Unknown glTF accessor component type: {}", datatype)};
    }
}

usize ModelLoader::glTF_componentElements(std::string_view componentType) {
    if (componentType == "SCALAR") {
        return 1;
    } else if (componentType == "VEC2") {
        return 2;
    } else if (componentType == "VEC3") {
        return 3;
    } else if (componentType == "VEC4") {
        return 4;
    } else if (componentType == "MAT2") {
        return 4;
    } else if (componentType == "MAT3") {
        return 9;
    } else if (componentType == "MAT4") {
        return 16;
    } else {
        throw Exception{std::format("Unknown glTF component type: {}", componentType)};
    }
}

std::string ModelLoader::glTF_imageKey(std::filesystem::path path) {
    return path.string();
}

std::string ModelLoader::glTF_embeddedImageKey(uint32 textureSource, uint32 bufferView) {
    return std::format("{}/embedded/{}_{}", path().parent_path().string(), textureSource, bufferView);
}

std::string ModelLoader::glTF_colorKey(uint8 color[4]) {
    return std::format("{}/solid_color/{}_{}_{}_{}",
                       path().parent_path().string(),
                       static_cast<int>(color[0]),
                       static_cast<int>(color[1]),
                       static_cast<int>(color[2]),
                       static_cast<int>(color[3]));
}

std::string ModelLoader::glTF_vertexBufferKey(uint32 index) {
    return std::format("{}/vertices/{}", path().parent_path().string(), index);
}

std::string ModelLoader::glTF_indexBufferKey(uint32 index) {
    return std::format("{}/indices/{}", path().parent_path().string(), index);
}

} // namespace R3
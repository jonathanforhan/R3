#include "ModelLoader.hpp"

#include <array>
#include <cstddef>
#include <filesystem>
#include <format>
#include <map>
#include <optional>
#include <span>
#include <string>
#include <string_view>
#include <utility>
#include <vector>
#include <vulkan/vulkan_core.h>
#include <entt/resource/resource.hpp>
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

Entity ModelLoader::glTFLoad(const std::filesystem::path& path) {
    LOG_INFO("Loading model: {}", path.string());
    m_path = path;

    glTF::Model model = glTF::ModelImporter().import(path);

    Entity root = World()->registry().create();
    World()->registry().emplace<TransformComponent>(root); // give root node a default transform

    vulkan::RenderContext& ctx = static_cast<vulkan::RenderContext&>(Engine()->context());

    m_cmd = &ctx.graphicsCommandBuffer();
    m_cmd->begin();

    for (glTF::Scene& scene : model.root.scenes) {
        for (uint32 iNode : scene.nodes) {
            glTF_processNode(root, model, model.root.nodes[iNode]);
        }
    }

    m_cmd->end();
    m_cmd->submitSync();

    return root;
}

void ModelLoader::glTF_processNode(Entity entity, glTF::Model& model, glTF::Node& node) {
    Entity parent = entity;

    Entity child{World()->registry().create()};
    World()->registry().get_or_emplace<HierarchyComponent>(parent).children.push_back(child); // add child to parent

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

    World()->registry().emplace<TransformComponent>(child).transform() = local;

    if (node.mesh) {
        if (model.root.meshes[*node.mesh].primitives.size() > 1) {
            // this node is a parent for multiple mesh primitives, give it a hierarchy component
            World()->registry().get_or_emplace<HierarchyComponent>(child).parent = parent;
        }
        glTF_processMesh(child, model, model.root.meshes[*node.mesh]);
    }

    if (!node.children.empty()) {
        // set parent of this node
        World()->registry().get_or_emplace<HierarchyComponent>(child).parent = parent;

        for (uint32 iChild : node.children) {
            glTF_processNode(child, model, model.root.nodes[iChild]);
        }
    }
}

void ModelLoader::glTF_processMesh(Entity entity, glTF::Model& model, glTF::Mesh& mesh) {
    bool isParent = mesh.primitives.size() > 1;

    for (glTF::MeshPrimitive& primitive : mesh.primitives) {
        R3_ASSERT(primitive.mode == glTF::TRIANGLES && "Only TRIANGLES mode is supported");

        //--- Vertices
        std::vector<fvec3> positions;
        R3_ASSERT(primitive.attributes.contains(glTF::POSITION));
        glTF_readAccessor(model, primitive.attributes[glTF::POSITION], positions);

        std::vector<fvec3> normals;
        if (primitive.attributes.contains(glTF::NORMAL)) {
            glTF_readAccessor(model, primitive.attributes[glTF::NORMAL], normals);
        }

        std::vector<fvec2> texCoords;
        if (primitive.attributes.contains(glTF::TEXCOORD_0)) {
            glTF_readAccessor(model, primitive.attributes[glTF::TEXCOORD_0], texCoords);
        }

        std::vector<ivec4> joints;
        if (primitive.attributes.contains(glTF::JOINTS_0)) {
            usize index = primitive.attributes[glTF::JOINTS_0];
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
        if (primitive.attributes.contains(glTF::WEIGHTS_0)) {
            usize index = primitive.attributes[glTF::WEIGHTS_0];

            if (glTF_sizeof(model.root.accessors[index].componentType) == sizeof(uint8)) {
                glTF_readAccessor<fvec4, u8vec4>(model, index, weights);
            } else if (glTF_sizeof(model.root.accessors[index].componentType) == sizeof(uint16)) {
                glTF_readAccessor<fvec4, u16vec4>(model, index, weights);
            } else {
                glTF_readAccessor(model, index, weights);
            }
        }

        std::vector<Vertex> vertices(positions.size());
        for (usize i = 0; i < vertices.size(); i++) {
            vertices[i].position      = positions[i];
            vertices[i].normal        = i < normals.size() ? normals[i] : fvec3(0.0f);
            vertices[i].textureCoords = i < texCoords.size() ? texCoords[i] : fvec2(0.0f);
            vertices[i].boneIDs       = i < joints.size() ? joints[i] : ivec4(-1);
            vertices[i].weights       = i < weights.size() ? weights[i] : fvec4(0.0f);
        }

        //--- Indices
        std::vector<uint32> indices;
        if (primitive.indices) {
            const glTF::Accessor& accessor = model.root.accessors[*primitive.indices];

            switch (glTF_sizeof(accessor.componentType)) {
                case sizeof(uint8):
                    glTF_readAccessor<uint32, uint8>(model, *primitive.indices, indices);
                    break;
                case sizeof(uint16):
                    glTF_readAccessor<uint32, uint16>(model, *primitive.indices, indices);
                    break;
                case sizeof(uint32):
                    glTF_readAccessor(model, *primitive.indices, indices);
                    break;
                default:
                    throw Exception{std::format("unsupported index datatype {}", accessor.componentType)};
            }
        } else {
            LOG_VERBOSE("processed mesh does not contain indices");
        }

        std::string parentPath     = m_path.parent_path().string();
        std::string idVertexBuffer = std::format("{}/vertices/{}", parentPath, primitive.attributes[glTF::POSITION]);
        std::string idindexBuffer  = std::format("{}/indices/{}", parentPath, primitive.indices.value_or(0));
        LOG_INFO("importing mesh {}, {}", idVertexBuffer, idindexBuffer);

        auto&& [vbo, vboLoaded] = ResourceManager()->loadBuffer(
            std::string_view(idVertexBuffer), nullptr, vertices.size() * sizeof(Vertex), BufferPreset::DeviceVertex);

        auto&& [ibo, iboLoaded] = ResourceManager()->loadBuffer(
            std::string_view(idindexBuffer), nullptr, indices.size() * sizeof(uint32), BufferPreset::DeviceIndex);

        if (vboLoaded || iboLoaded) {
            if (vboLoaded) {
                vulkan::Buffer* vertexStagingBuffer = ResourceManager()->newFrameScopedObject<vulkan::Buffer>();
                VkBufferCopy* vertexCopyRegion      = ResourceManager()->newFrameScopedObject<VkBufferCopy>();

                *vertexStagingBuffer = vulkan::Buffer{std::span<const Vertex>{vertices}, BufferPreset::Staging};
                *vertexCopyRegion    = {0, 0, vertices.size() * sizeof(Vertex)};
                m_cmd->copyBuffer(vertexStagingBuffer->buffer(), vbo->buffer(), {vertexCopyRegion, 1});
            }

            if (iboLoaded) {
                vulkan::Buffer* indexStagingBuffer = ResourceManager()->newFrameScopedObject<vulkan::Buffer>();
                VkBufferCopy* indexCopyRegion      = ResourceManager()->newFrameScopedObject<VkBufferCopy>();

                *indexStagingBuffer = vulkan::Buffer{std::span<const uint32>{indices}, BufferPreset::Staging};
                *indexCopyRegion    = {0, 0, indices.size() * sizeof(uint32)};
                m_cmd->copyBuffer(indexStagingBuffer->buffer(), ibo->buffer(), {indexCopyRegion, 1});
            }
        }

        if (!isParent) {
            World()->registry().emplace<MeshComponent>(
                entity, std::move(vbo), vertices.size(), std::move(ibo), indices.size());

            if (primitive.material) {
                glTF_processMaterial(entity, model, model.root.materials[*primitive.material]);
            }
        } else {
            Entity child = World()->registry().create();
            World()->registry().emplace<TransformComponent>(child);
            World()->registry().get<HierarchyComponent>(entity).children.push_back(child);
            World()->registry().emplace<MeshComponent>(
                child, std::move(vbo), vertices.size(), std::move(ibo), indices.size());

            if (primitive.material) {
                glTF_processMaterial(child, model, model.root.materials[*primitive.material]);
            }
        }
    }
}

void ModelLoader::glTF_processAnimations(Entity entity, glTF::Model& model) {
    /* TODO */
}

void ModelLoader::glTF_processSkeleton(Entity entity, glTF::Model& model) {
    /* TODO */
}

void ModelLoader::glTF_processJoint(Entity entity, glTF::Model& model, usize modelIndex, usize parentJoint) {
    /* TODO */
}

void ModelLoader::glTF_processMaterial(Entity entity, glTF::Model& model, glTF::Material& material) {
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

void ModelLoader::glTF_processTexture(Entity entity, glTF::Model& model, glTF::Texture& texture, TextureType type) {
    if (!texture.source) {
        return;
    }

    const glTF::Image& image = model.root.images[*texture.source];

    Handle<vulkan::Texture> hTexture;
    std::string name;

    if (!image.uri.empty()) {
        std::filesystem::path imagePath = m_path;
        imagePath.replace_filename(image.uri);

        name = imagePath.string();
        LOG_INFO("importing texture {}", name);

        auto&& [tex, texLoaded] = ResourceManager()->loadTexture(std::string_view(name));

        if (texLoaded) {
            vulkan::Buffer* stagingBuffer = ResourceManager()->newFrameScopedObject<vulkan::Buffer>();

            *tex = vulkan::Texture{*m_cmd, imagePath, type, *stagingBuffer};
        }

        hTexture = std::move(tex);
    } else {
        glTF::BufferView& bufferView = model.root.bufferViews[*image.bufferView];
        const std::byte* data        = &(model.bin[bufferView.buffer][bufferView.byteOffset]);

        name = std::format("{}/embedded/{}/{}", m_path.parent_path().string(), *texture.source, *image.bufferView);
        LOG_INFO("importing texture {}", name);
        auto&& [tex, texLoaded] = ResourceManager()->loadTexture(std::string_view(name));

        if (texLoaded) {
            vulkan::Buffer* stagingBuffer = ResourceManager()->newFrameScopedObject<vulkan::Buffer>();

            *tex = vulkan::Texture{*m_cmd, data, bufferView.byteLength, type, *stagingBuffer};
        }

        hTexture = std::move(tex);
    }

    MaterialComponent& mat = World()->registry().get_or_emplace<MaterialComponent>(entity);
    uint32 slot            = ResourceManager()->bindTexture(std::string_view(name), *hTexture);
    mat.setTextureSlot(type, slot);

    TextureLifetimeComponent& textureLifetime = World()->registry().get_or_emplace<TextureLifetimeComponent>(entity);
    textureLifetime.textures.emplace_back(std::move(hTexture)); // ensure texture lives as long as entity
}

void ModelLoader::glTF_processTexture(Entity entity, glTF::Model& model, uint8 color[4], TextureType type) {
    std::string name = std::format("{}/solid_colors/{}_{}_{}_{}_{}",
                                   m_path.parent_path().string(),
                                   static_cast<int>((uint16)type),
                                   static_cast<int>(color[0]),
                                   static_cast<int>(color[1]),
                                   static_cast<int>(color[2]),
                                   static_cast<int>(color[3]));

    LOG_INFO("importing texture {}", name);
    auto&& [tex, texLoaded] = ResourceManager()->loadTexture(std::string_view(name));

    if (texLoaded) {
        vulkan::Buffer* stagingBuffer = ResourceManager()->newFrameScopedObject<vulkan::Buffer>();

        *tex = vulkan::Texture{*m_cmd, (const std::byte*)color, 1, 1, type, *stagingBuffer};
    }

    MaterialComponent& mat = World()->registry().get_or_emplace<MaterialComponent>(entity);
    uint32 slot            = ResourceManager()->bindTexture(std::string_view(name), *tex);
    mat.setTextureSlot(type, slot);

    TextureLifetimeComponent& textureLifetime = World()->registry().get_or_emplace<TextureLifetimeComponent>(entity);
    textureLifetime.textures.emplace_back(std::move(tex)); // ensure texture lives as long as entity
}

void ModelLoader::glTF_processTextureInfo(Entity entity,
                                          glTF::Model& model,
                                          glTF::TextureInfo& textureInfo,
                                          TextureType type) {
    glTF::Texture& texture = model.root.textures[textureInfo.index];
    glTF_processTexture(entity, model, texture, type);
}

void ModelLoader::glTF_processTextureInfo(Entity entity,
                                          glTF::Model& model,
                                          glTF::NormalTextureInfo& textureInfo,
                                          TextureType type) {
    glTF::TextureInfo adapter{.index = textureInfo.index};
    glTF_processTextureInfo(entity, model, adapter, type);
}

void ModelLoader::glTF_processTextureInfo(Entity entity,
                                          glTF::Model& model,
                                          glTF::OcclusionTextureInfo& textureInfo,
                                          TextureType type) {
    glTF::TextureInfo adapter{.index = textureInfo.index};
    glTF_processTextureInfo(entity, model, adapter, type);
}

template <typename T, typename U>
void ModelLoader::glTF_readAccessor(glTF::Model& model, usize iAccessor, std::vector<T>& out) {
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

} // namespace R3
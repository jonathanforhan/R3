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

    m_entity = World()->registry().create();
    World()->registry().emplace<TransformComponent>(m_entity); // give root node a default transform

    vulkan::RenderContext& ctx = static_cast<vulkan::RenderContext&>(Engine()->context());

    m_cmd = &ctx.graphicsCommandBuffer();
    m_cmd->begin();

    for (glTF::Scene& scene : model.root.scenes) {
        for (uint32 iNode : scene.nodes) {
            glTF_processNode(model, model.root.nodes[iNode], iNode);
        }
    }

    m_cmd->end();
    m_cmd->submitSync();

    return m_entity;
}

void ModelLoader::glTF_processNode(glTF::Model& model, glTF::Node& node, usize id) {
    Entity parent = m_entity;

    Entity child{World()->registry().create()};
    HierarchyComponent& hier = World()->registry().get_or_emplace<HierarchyComponent>(parent);
    hier.children.push_back(child); // add child to parent

    m_entity = child; // set current entity to child for processing

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
        glTF_processMesh(model, model.root.meshes[*node.mesh], *node.mesh);
    }

    if (!node.children.empty()) {
        // set parent of this node
        World()->registry().get_or_emplace<HierarchyComponent>(child).parent = parent;

        for (uint32 iChild : node.children) {
            glTF_processNode(model, model.root.nodes[iChild], iChild);
        }
    }

    m_entity = parent; // restore current entity to parent
}

void ModelLoader::glTF_processMesh(glTF::Model& model, glTF::Mesh& mesh, usize id) {
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
            std::vector<u16vec4> jointIndices;

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

        if (primitive.material) {
            glTF_processMaterial(model, model.root.materials[*primitive.material], *primitive.material);
        }

        std::string idVertexBuffer = std::format("{}/vertices/{}", m_path.parent_path().string(), id);
        std::string idindexBuffer  = std::format("{}/indices/{}", m_path.parent_path().string(), id);
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

        World()->registry().emplace<MeshComponent>(
            m_entity, std::move(vbo), vertices.size(), std::move(ibo), indices.size());
    }
}

void ModelLoader::glTF_processAnimations(glTF::Model& model, usize id) {
    /* TODO */
}

void ModelLoader::glTF_processSkeleton(glTF::Model& model, usize id) {
    /* TODO */
}

void ModelLoader::glTF_processJoint(glTF::Model& model, usize modelIndex, usize parentJoint, usize id) {
    /* TODO */
}

void ModelLoader::glTF_processMaterial(glTF::Model& model, glTF::Material& material, usize id) {
    if (material.emissiveTexture) {
        glTF_processTextureInfo(model, *material.emissiveTexture, TextureType::Emissive, id);
    }

    if (material.occlusionTexture) {
        glTF_processTextureInfo(model, *material.occlusionTexture, TextureType::AmbientOcclusion, id);
    }

    if (material.normalTexture) {
        glTF_processTextureInfo(model, *material.normalTexture, TextureType::Normal, id);
    }

    if (material.pbrMetallicRoughness->metallicRoughnessTexture) {
        glTF_processTextureInfo(
            model, *material.pbrMetallicRoughness->metallicRoughnessTexture, TextureType::MetallicRoughness, id);
    }

    if (material.pbrMetallicRoughness->baseColorTexture) {
        glTF_processTextureInfo(model, *material.pbrMetallicRoughness->baseColorTexture, TextureType::Albedo, id);
    } else {
        uint8 color[4] = {
            static_cast<uint8>(material.pbrMetallicRoughness->baseColorFactor[0] * 255.0f),
            static_cast<uint8>(material.pbrMetallicRoughness->baseColorFactor[1] * 255.0f),
            static_cast<uint8>(material.pbrMetallicRoughness->baseColorFactor[2] * 255.0f),
            static_cast<uint8>(material.pbrMetallicRoughness->baseColorFactor[3] * 255.0f),
        };
        glTF_processTexture(model, color, TextureType::Albedo, id);
    }
}

void ModelLoader::glTF_processTexture(glTF::Model& model, glTF::Texture& texture, TextureType type, usize id) {
    if (!texture.source) {
        return;
    }

    id += (usize)type * 1000; // unique to each type for a given material

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

        name = std::format("{}/images/{}", m_path.parent_path().string(), id);
        LOG_INFO("importing texture {}", name);
        auto&& [tex, texLoaded] = ResourceManager()->loadTexture(std::string_view(name));

        if (texLoaded) {
            vulkan::Buffer* stagingBuffer = ResourceManager()->newFrameScopedObject<vulkan::Buffer>();

            *tex = vulkan::Texture{*m_cmd, data, bufferView.byteLength, type, *stagingBuffer};
        }

        hTexture = std::move(tex);
    }

    MaterialComponent& mat = World()->registry().get_or_emplace<MaterialComponent>(m_entity);
    uint32 slot            = ResourceManager()->bindTexture(std::string_view(name), *hTexture);
    mat.setTextureSlot(type, slot);

    TextureLifetimeComponent& textureLifetime = World()->registry().get_or_emplace<TextureLifetimeComponent>(m_entity);
    textureLifetime.textures.emplace_back(std::move(hTexture)); // ensure texture lives as long as entity
}

void ModelLoader::glTF_processTexture(glTF::Model& model, uint8 color[4], TextureType type, usize id) {
    id += (usize)type * 1000; // unique to each type for a given material

    std::string name = std::format("{}/colors/{}", m_path.parent_path().string(), id);
    LOG_INFO("importing texture {}", name);
    auto&& [tex, texLoaded] = ResourceManager()->loadTexture(std::string_view(name));

    if (texLoaded) {
        vulkan::Buffer* stagingBuffer = ResourceManager()->newFrameScopedObject<vulkan::Buffer>();

        *tex = vulkan::Texture{*m_cmd, (const std::byte*)color, 1, 1, type, *stagingBuffer};
    }

    MaterialComponent& mat = World()->registry().get_or_emplace<MaterialComponent>(m_entity);
    uint32 slot            = ResourceManager()->bindTexture(std::string_view(name), *tex);
    mat.setTextureSlot(type, slot);

    TextureLifetimeComponent& textureLifetime = World()->registry().get_or_emplace<TextureLifetimeComponent>(m_entity);
    textureLifetime.textures.emplace_back(std::move(tex)); // ensure texture lives as long as entity
}

void ModelLoader::glTF_processTextureInfo(glTF::Model& model,
                                          glTF::TextureInfo& textureInfo,
                                          TextureType type,
                                          usize id) {
    glTF::Texture& texture = model.root.textures[textureInfo.index];
    glTF_processTexture(model, texture, type, id);
}

void ModelLoader::glTF_processTextureInfo(glTF::Model& model,
                                          glTF::NormalTextureInfo& textureInfo,
                                          TextureType type,
                                          usize id) {
    glTF::TextureInfo adapter{.index = textureInfo.index};
    glTF_processTextureInfo(model, adapter, type, id);
}

void ModelLoader::glTF_processTextureInfo(glTF::Model& model,
                                          glTF::OcclusionTextureInfo& textureInfo,
                                          TextureType type,
                                          usize id) {
    glTF::TextureInfo adapter{.index = textureInfo.index};
    glTF_processTextureInfo(model, adapter, type, id);
}

template <typename T, typename U>
void ModelLoader::glTF_readAccessor(glTF::Model& model, usize iAccessor, std::vector<T>& out) {
    R3_ASSERT(out.empty() && "out data must be empty");

    const glTF::Accessor& accessor = model.root.accessors[iAccessor];
    R3_ASSERT(glTF_sizeof(accessor.componentType) * glTF_componentElements(accessor.type) == sizeof(U));

    if (!accessor.bufferView) {
        throw Exception{"Sparse accessors are not supported"};
    }

    const glTF::BufferView& bufferView = model.root.bufferViews[*accessor.bufferView];

    const uint32 offset = accessor.byteOffset + bufferView.byteOffset;

    out.reserve(accessor.count);
    for (usize i = 0; i < accessor.count; i++) {
        // read as U but cast to T
        out.emplace_back(
            static_cast<T>(*reinterpret_cast<const U*>(&model.bin[bufferView.buffer][offset + (i * sizeof(U))])));
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
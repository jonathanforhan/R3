#include "ModelLoader.hpp"

#include <array>
#include <filesystem>
#include <format>
#include <map>
#include <optional>
#include <span>
#include <string_view>
#include <utility>
#include <vector>
#include <vulkan/vulkan_core.h>
#include <entt/resource/resource.hpp>
#include "api/Assert.hpp"
#include "api/Exception.hpp"
#include "api/Types.hpp"
#include "components/MeshComponent.hpp"
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

namespace R3 {

Entity ModelLoader::glTFLoad(const std::filesystem::path& path) {
    glTF::Model model = glTF::ModelImporter().import(path);

    m_entity = World()->registry().create();

    m_directory = path.parent_path();

    for (glTF::Scene& scene : model.root.scenes) {
        for (uint32 iNode : scene.nodes) {
            glTF_processNode(model, model.root.nodes[iNode]);
        }
    }

    return m_entity;
}

void ModelLoader::glTF_processNode(glTF::Model& model, glTF::Node& node) {
    for (uint32 iChild : node.children) {
        glTF_processNode(model, model.root.nodes[iChild]);
    }

    if (node.mesh) {
        glTF_processMesh(model, model.root.meshes[*node.mesh]);
    }
}

void ModelLoader::glTF_processMesh(glTF::Model& model, glTF::Mesh& mesh) {
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
            glTF_processMaterial(model, model.root.materials[*primitive.material]);
        }

        vulkan::Buffer vertexStagingBuffer{std::span<const Vertex>{vertices}, BufferPreset::Staging};
        vulkan::Buffer indexStagingBuffer{std::span<const uint32>{indices}, BufferPreset::Staging};

        Handle<vulkan::Buffer> vbo =
            ResourceManager()->loadBuffer("vbo", nullptr, vertices.size() * sizeof(Vertex), BufferPreset::DeviceVertex);
        Handle<vulkan::Buffer> ibo =
            ResourceManager()->loadBuffer("ibo", nullptr, indices.size() * sizeof(uint32), BufferPreset::DeviceIndex);

        const VkBufferCopy vertexCopyRegion = {0, 0, vertices.size() * sizeof(Vertex)};
        const VkBufferCopy indexCopyRegion  = {0, 0, indices.size() * sizeof(uint32)};

        vulkan::RenderContext& ctx = static_cast<vulkan::RenderContext&>(Engine()->context());
        vulkan::CommandBuffer& cmd = ctx.graphicsCommandBuffer();
        cmd.begin();
        {
            cmd.copyBuffer(vertexStagingBuffer.buffer(), vbo->buffer(), {&vertexCopyRegion, 1});
            cmd.copyBuffer(indexStagingBuffer.buffer(), ibo->buffer(), {&indexCopyRegion, 1});
        }
        cmd.end();
        cmd.submitSync(ctx.graphicsQueue());

        World()->registry().emplace<MeshComponent>(
            m_entity, std::move(vbo), vertices.size(), std::move(ibo), indices.size());
    }
}

void ModelLoader::glTF_processAnimations(glTF::Model& model) {
    /* TODO */
}

void ModelLoader::glTF_processSkeleton(glTF::Model& model) {
    /* TODO */
}

void ModelLoader::glTF_processJoint(glTF::Model& model, usize modelIndex, usize parentJoint) {
    /* TODO */
}

void ModelLoader::glTF_processMaterial(glTF::Model& model, glTF::Material& material) {
    if (material.emissiveTexture) {
        glTF_processTexture(model, *material.emissiveTexture, TextureType::Emissive);
    }

    if (material.occlusionTexture) {
        glTF_processTexture(model, *material.occlusionTexture, TextureType::AmbientOcclusion);
    }

    if (material.normalTexture) {
        glTF_processTexture(model, *material.normalTexture, TextureType::Normal);
    }

    if (material.pbrMetallicRoughness->metallicRoughnessTexture) {
        glTF_processTexture(
            model, *material.pbrMetallicRoughness->metallicRoughnessTexture, TextureType::MetallicRoughness);
    }

    if (material.pbrMetallicRoughness->baseColorTexture) {
        glTF_processTexture(model, *material.pbrMetallicRoughness->baseColorTexture, TextureType::Albedo);
    } else {
        uint8 color[4] = {
            static_cast<uint8>(material.pbrMetallicRoughness->baseColorFactor[0] * 255.0f),
            static_cast<uint8>(material.pbrMetallicRoughness->baseColorFactor[1] * 255.0f),
            static_cast<uint8>(material.pbrMetallicRoughness->baseColorFactor[2] * 255.0f),
            static_cast<uint8>(material.pbrMetallicRoughness->baseColorFactor[3] * 255.0f),
        };
        glTF_processTexture(model, color, TextureType::Albedo);
    }
}

void ModelLoader::glTF_processTexture(glTF::Model& model, uint8 color[4], TextureType type) {
    /*TODO*/
}

void ModelLoader::glTF_processTexture(glTF::Model& model, glTF::TextureInfo& textureInfo, TextureType type) {
    /*TODO*/
}

void ModelLoader::glTF_processTexture(glTF::Model& model, glTF::NormalTextureInfo& textureInfo, TextureType type) {
    /*TODO*/
}

void ModelLoader::glTF_processTexture(glTF::Model& model, glTF::OcclusionTextureInfo& textureInfo, TextureType type) {
    /*TODO*/
}

void ModelLoader::glTF_preProcessTextures(glTF::Model& model) {
    /*TODO*/
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
        out.emplace_back(static_cast<T>(*(const U*)(&model.bin[offset + (i * sizeof(U))])));
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
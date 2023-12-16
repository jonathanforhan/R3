#include "ModelComponent.hpp"
#include "api/Check.hpp"
#include "api/Ensure.hpp"
#include "api/Log.hpp"
#include "api/Math.hpp"
#include "core/Engine.hpp"
#include "media/glTF/glTF-Model.hxx"
#include "systems/ModelSystem.hpp"

namespace R3 {

ModelComponent::ModelComponent(const std::string& path)
    : m_directory(),
      m_file() {
    usize split = path.find_last_of('/') + 1;
    m_directory = path.substr(0, split);
    m_file = path.substr(split);

    glTF::Model gltf(path);

    for (auto& scene : gltf.scenes) {
        for (uint32 iNode : scene.nodes) {
            processNode(&gltf, &gltf.nodes[iNode]);
        }
    }

    for (auto& ext : gltf.extensionsUsed) {
        LOG(Info, ext);
    }

    // Engine::activeScene().addSystem<ModelSystem>();
}

void ModelComponent::processNode(glTF::Model* model, glTF::Node* node) {
    if (node->mesh != glTF::UNDEFINED) {
        processMesh(model, node, &model->meshes[node->mesh]);
    }

    for (uint32 child : node->children) {
        processNode(model, &model->nodes[child]);
    }
}

void ModelComponent::processMesh(glTF::Model* model, glTF::Node* node, glTF::Mesh* mesh) {
    auto datatypeSize = [](uint32 datatype) -> usize {
        switch (datatype) {
            case glTF::UNSIGNED_BYTE:
                return sizeof(uint8);
            case glTF::UNSIGNED_SHORT:
                return sizeof(uint16);
            case glTF::UNSIGNED_INT:
                return sizeof(uint32);
            case glTF::FLOAT:
                return sizeof(float);
            default:
                ENSURE(false);
        }
    };

    auto populateVertexAttrib = [=](const glTF::MeshPrimitive& primitive, auto& vec, const char* attrib) {
        if (primitive.attributes.HasMember(attrib)) {
            glTF::Accessor& accessor = model->accessors[primitive.attributes[attrib].GetUint()];
            glTF::BufferView& bufferView = model->bufferViews[accessor.bufferView];

            uint32 nComponents{};
            if (accessor.type == glTF::VEC3)
                nComponents = 3;
            else if (accessor.type == glTF::VEC2)
                nComponents = 2;

            usize nBytes = usize(accessor.byteOffset) + bufferView.byteOffset;
            usize nSize = datatypeSize(accessor.componentType);
            vec.resize((usize)accessor.count);
            memcpy(vec.data(), model->buffer().data() + nBytes, accessor.count * nSize * nComponents);
        }
    };

    for (auto& primitive : mesh->primitives) {
        std::vector<vec3> positions;
        populateVertexAttrib(primitive, positions, glTF::POSITION);

        for (auto& position : positions) {
            // A node MAY have either a matrix or any combination of translation/rotation/scale (TRS) properties.
            // TRS properties are converted to matrices and postmultiplied in the T * R * S order to compose the
            // transformation matrix; first the scale is applied to the vertices, then the rotation, and then the
            // translation. If none are provided, the transform is the identity. When a node is targeted for animation
            // (referenced by an animation.channel.target), matrix MUST NOT be present
            mat4 t;
            for (uint32 i = 0; i < 16; i++)
                t[i / 4][i % 4] = node->matrix[i];

            vec3 T = vec3(node->translation[0], node->translation[1], node->translation[2]);
            t *= glm::translate(t, T);

            vec3 R = vec3(node->rotation[0], node->rotation[1], node->rotation[2]);
            float theta = node->rotation[3];
            if (R.x || R.y || R.z)
                t *= glm::rotate(t, theta, R);

            vec3 S = vec3(node->scale[0], node->scale[1], node->scale[2]);
            t *= glm::scale(t, S);

            position = mat3(t) * position;
        }

        std::vector<vec3> normals;
        populateVertexAttrib(primitive, normals, glTF::NORMAL);

        std::vector<vec2> texCoords;
        populateVertexAttrib(primitive, texCoords, glTF::TEXCOORD_0);

        std::vector<uint32> indices;
        if (primitive.indices != glTF::UNDEFINED) {
            glTF::Accessor& accessor = model->accessors[primitive.indices];
            glTF::BufferView& bufferView = model->bufferViews[accessor.bufferView];

            usize nBytes = (usize)accessor.byteOffset + bufferView.byteOffset;
            usize nSize = datatypeSize(accessor.componentType);

            indices.resize(accessor.count);
            if (nSize == 1) {
                for (uint32 i = 0; i < accessor.count; i++)
                    indices[i] = *(uint8*)(&model->buffer()[nBytes + i * nSize]);
            } else if (nSize == 2) {
                for (uint32 i = 0; i < accessor.count; i++)
                    indices[i] = *(uint16*)(&model->buffer()[nBytes + i * nSize]);
            } else if (nSize == 4) {
                for (uint32 i = 0; i < accessor.count; i++)
                    indices[i] = *(uint32*)(&model->buffer()[nBytes + i * nSize]);
            } else if (nSize == 8) {
                for (uint32 i = 0; i < accessor.count; i++)
                    indices[i] = static_cast<uint32>(*(uint64*)(&model->buffer()[nBytes + i * nSize]));
            }
        }

        CHECK(primitive.mode == glTF::TRIANGLES);

        std::vector<Vertex> vertices(positions.size());
        for (usize i = 0; i < vertices.size(); i++) {
            vertices[i].position = positions[i];

            if (i < normals.size())
                vertices[i].normal = normals[i];

            if (i < texCoords.size())
                vertices[i].textureCoords = texCoords[i];
        }

        auto& renderer = Engine::renderer();

        m_meshes.emplace_back(MeshSpecification{
            .physicalDevice = renderer.physicalDevice(),
            .logicalDevice = renderer.logicalDevice(),
            .commandPool = renderer.commandPool(),
            .vertices = vertices,
            .indices = indices,
        });

        if (primitive.material != glTF::UNDEFINED) {
            processMaterial(model, &model->materials[primitive.material]);
        }
    }
}

void ModelComponent::processMaterial(glTF::Model* model, glTF::Material* material) {
    if (material->pbrMetallicRoughness.has_value()) {
        if (material->emissiveTexture.has_value())
            processTexture(model, &*material->emissiveTexture, TextureType::Emissive);

        if (material->occlusionTexture.has_value())
            processTexture(model, &*material->occlusionTexture, TextureType::AmbientOcclusion);

        if (material->normalTexture.has_value())
            processTexture(model, &*material->normalTexture, TextureType::Normal);

        if (material->pbrMetallicRoughness->metallicRoughnessTexture.has_value())
            processTexture(
                model, &*material->pbrMetallicRoughness->metallicRoughnessTexture, TextureType::MetallicRoughness);

        if (material->pbrMetallicRoughness->baseColorTexture.has_value())
            processTexture(model, &*material->pbrMetallicRoughness->baseColorTexture, TextureType::Albedo);
    }
}

void ModelComponent::processTexture(glTF::Model* model, glTF::TextureInfo* textureInfo, TextureType type) {
    if (m_loadedTextures.contains(textureInfo->index)) {
        m_meshes.back().addTextureIndex(textureInfo->index);
        return;
    } else {
        m_loadedTextures.emplace(textureInfo->index, m_textures.size());
    }

    glTF::Texture& texture = model->textures[textureInfo->index];

    auto& renderer = Engine::renderer();

    if (texture.source != glTF::UNDEFINED) {
        m_meshes.back().addTextureIndex(static_cast<uint32>(m_textures.size()));
        glTF::Image& image = model->images[texture.source];

        if (!image.uri.empty()) {
            m_textures.emplace_back(TextureBufferSpecification{
                .physicalDevice = renderer.physicalDevice(),
                .logicalDevice = renderer.logicalDevice(),
                .swapchain = renderer.swapchain(),
                .commandPool = renderer.commandPool(),
                .path = std::string(m_directory + image.uri).c_str(),
                .type = type,
            });
        } else {
            glTF::BufferView& bufferView = model->bufferViews[image.bufferView];
            const uint8* data = model->buffer().data() + bufferView.byteOffset;

            m_textures.emplace_back(TextureBufferSpecification{
                .physicalDevice = renderer.physicalDevice(),
                .logicalDevice = renderer.logicalDevice(),
                .swapchain = renderer.swapchain(),
                .commandPool = renderer.commandPool(),
                .width = bufferView.byteLength,
                .height = 0,
                .data = data,
                .type = type,
            });
        }
    }
}

void ModelComponent::processTexture(glTF::Model* model, glTF::NormalTextureInfo* textureInfo, TextureType type) {
    glTF::TextureInfo adapter{.index = textureInfo->index};
    processTexture(model, &adapter, type);
}

void ModelComponent::processTexture(glTF::Model* model, glTF::OcclusionTextureInfo* textureInfo, TextureType type) {
    glTF::TextureInfo adapter{.index = textureInfo->index};
    processTexture(model, &adapter, type);
}

} // namespace R3
#include "render/ModelLoader.hpp"

#include <R3>
#include "ResourceManager.hxx"
#include "core/Scene.hpp"
#include "media/glTF/glTF-Extensions.hxx"
#include "media/glTF/glTF-Model.hxx"
#include "private/render/ResourceManager.hxx"
#include "render/CommandPool.hpp"
#include "render/ShaderObjects.hpp"

namespace R3 {

static ResourceManager* resourceManager;

ModelLoader::ModelLoader(const ModelLoaderSpecification& spec)
    : m_physicalDevice(&spec.physicalDevice),
      m_logicalDevice(&spec.logicalDevice),
      m_swapchain(&spec.swapchain),
      m_renderPass(&spec.renderPass),
      m_commandPool(&spec.commandPool),
      m_storageBuffer(&spec.storageBuffer) {}

void ModelLoader::load(const std::string& path, ModelComponent& model) {
    usize split = path.find_last_of('/') + 1;
    m_directory = path.substr(0, split);
    auto file = path.substr(split);

    glTF::Model gltf(m_directory + file);

    resourceManager = static_cast<ResourceManager*>(CurrentScene->resourceManager);

    for (auto& scene : gltf.scenes) {
        for (uint32 iNode : scene.nodes) {
            processNode(&gltf, &gltf.nodes[iNode]);
        }
    }

    for (auto& prototype : m_prototypes) {
        Mesh mesh;

        // Vertex Buffer
        mesh.vertexBuffer = prototype.vertexBuffer;

        // Index Buffer
        mesh.indexBuffer = prototype.indexBuffer;

        // Descriptor Set Layout Bindings
        std::vector<DescriptorSetLayoutBinding> layoutBindings = {
            {
                // Uniform Buffer Object
                .binding = 0,
                .type = DescriptorType::UniformBuffer,
                .count = 1,
                .stage = ShaderStage::Vertex,
            },
            {
                // Albedo
                .binding = 1,
                .type = DescriptorType::CombinedImageSampler,
                .count = 1,
                .stage = ShaderStage::Fragment,
            },
            {
                // Metallic Roughness
                .binding = 2,
                .type = DescriptorType::CombinedImageSampler,
                .count = 1,
                .stage = ShaderStage::Fragment,
            },
            {
                // Normal
                .binding = 3,
                .type = DescriptorType::CombinedImageSampler,
                .count = 1,
                .stage = ShaderStage::Fragment,
            },
            {
                // Ambient Occlusion
                .binding = 4,
                .type = DescriptorType::CombinedImageSampler,
                .count = 1,
                .stage = ShaderStage::Fragment,
            },
            {
                // Emissive
                .binding = 5,
                .type = DescriptorType::CombinedImageSampler,
                .count = 1,
                .stage = ShaderStage::Fragment,
            },
            {
                // Lighting
                .binding = 6,
                .type = DescriptorType::UniformBuffer,
                .count = 1,
                .stage = ShaderStage::Fragment,
            },
            {
                // MousePicker
                .binding = 7,
                .type = DescriptorType::StorageBuffer,
                .count = 1,
                .stage = ShaderStage::Fragment,
            },
        };

        // Descriptor Pool
        mesh.material.descriptorPool = resourceManager->allocateDescriptorPool({
            .logicalDevice = *m_logicalDevice,
            .descriptorSetCount = MAX_FRAMES_IN_FLIGHT,
            .layoutBindings = layoutBindings,
        });

        // Pipeline
        mesh.pipeline = resourceManager->allocateGraphicsPipeline({
            .physicalDevice = *m_physicalDevice,
            .logicalDevice = *m_logicalDevice,
            .swapchain = *m_swapchain,
            .renderPass = *m_renderPass,
            .descriptorSetLayout = resourceManager->getDescriptorPoolById(mesh.material.descriptorPool).layout(),
            .vertexBindingSpecification = Vertex::vertexBindingSpecification(),
            .vertexAttributeSpecification = Vertex::vertexAttributeSpecification(),
            .vertexShaderPath = "spirv/test.vert.spv",
            .fragmentShaderPath = "spirv/test.frag.spv",
            .msaa = true,
        });

        // Uniform
        for (auto i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
            mesh.material.uniforms[i] = resourceManager->allocateUniform({
                .physicalDevice = *m_physicalDevice,
                .logicalDevice = *m_logicalDevice,
                .bufferSize = sizeof(VertexUniformBufferObject),
            });
            mesh.material.uniforms[i + 3] = resourceManager->allocateUniform({
                .physicalDevice = *m_physicalDevice,
                .logicalDevice = *m_logicalDevice,
                .bufferSize = sizeof(FragmentUniformBufferObject),
            });
        }

        // Textures
        std::vector<TextureDescriptor> textureDescriptors;

        for (usize index : prototype.textureIndices) {
            auto& texture = resourceManager->getTextureById(m_textures[index]);
            auto type = texture.type();
            mesh.material.pbrFlags |= (1 << (uint32(type) - 1));

            switch (type) {
                case TextureType::Albedo:
                    mesh.material.textures.albedo = m_textures[index];
                    textureDescriptors.emplace_back(m_textures[index], 1);
                    break;
                case TextureType::MetallicRoughness:
                    mesh.material.textures.metallicRoughness = m_textures[index];
                    textureDescriptors.emplace_back(m_textures[index], 2);
                    break;
                case TextureType::Normal:
                    mesh.material.textures.normal = m_textures[index];
                    textureDescriptors.emplace_back(m_textures[index], 3);
                    break;
                case TextureType::AmbientOcclusion:
                    mesh.material.textures.ambientOcclusion = m_textures[index];
                    textureDescriptors.emplace_back(m_textures[index], 4);
                    break;
                case TextureType::Emissive:
                    mesh.material.textures.emissive = m_textures[index];
                    textureDescriptors.emplace_back(m_textures[index], 5);
                    break;
                default:
                    break;
            }
        }

        const uint32 data = 0x00FF'FFFF; // forfills glTF spec of white base color on missing pbrMetallicRoughness
        const TextureBufferSpecification nilTextureSpec = {
            .physicalDevice = *m_physicalDevice,
            .logicalDevice = *m_logicalDevice,
            .commandBuffer = m_commandPool->commandBuffers().front(),
            .width = 1,
            .height = 1,
            .raw = (const uint8*)&data,
            .type = TextureType::Nil,
        };

        for (auto i = 0U; i < PBR_TEXTURE_COUNT; i++) {
            if (!(mesh.material.pbrFlags & (1 << i))) {
                TextureBuffer::ID textureID = resourceManager->allocateTexture(nilTextureSpec);
                textureDescriptors.emplace_back(textureID, i + 1);
            }
        }

        // Bindings
        std::vector<UniformDescriptor> uniformDescriptors;
        for (uint32 i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
            uniformDescriptors.emplace_back(mesh.material.uniforms[i], 0);
            uniformDescriptors.emplace_back(mesh.material.uniforms[i + 3], 6);
        };

        StorageDescriptor storageDescriptors[] = {
            {*m_storageBuffer, 7},
            {*m_storageBuffer, 7},
            {*m_storageBuffer, 7},
        };

        auto& descriptorPool = resourceManager->getDescriptorPoolById(mesh.material.descriptorPool);
        auto& descriptorSets = descriptorPool.descriptorSets();

        for (uint32 i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
            descriptorSets[i].bindResources({uniformDescriptors, storageDescriptors, textureDescriptors});
        }

        model.m_meshes.emplace_back(std::move(mesh));
    }

    // save for reuse
    m_prototypes.clear();
    m_textures.clear();
    m_loadedTextures.clear();
}

void ModelLoader::processNode(glTF::Model* model, glTF::Node* node) {
    if (node->mesh != glTF::UNDEFINED) {
        processMesh(model, node, &model->meshes[node->mesh]);
    }

    for (uint32 child : node->children) {
        processNode(model, &model->nodes[child]);
    }
}

void ModelLoader::processMesh(glTF::Model* model, glTF::Node* node, glTF::Mesh* mesh) {
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

            uint32 nComponents = 0;
            if (accessor.type == glTF::VEC3) {
                nComponents = 3;
            } else if (accessor.type == glTF::VEC2) {
                nComponents = 2;
            }

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
            // A node MAY have either a matrix or any combination of translation/rotation/scale (TRS)
            // properties. TRS properties are converted to matrices and postmultiplied in the T * R * S order to
            // compose the transformation matrix; first the scale is applied to the vertices, then the rotation,
            // and then the translation. If none are provided, the transform is the identity. When a node is
            // targeted for animation (referenced by an animation.channel.target), matrix MUST NOT be present
            mat4 t = {};
            for (uint32 i = 0; i < 16; i++) {
                t[i / 4][i % 4] = node->matrix[i];
            }

            vec3 T = vec3(node->translation[0], node->translation[1], node->translation[2]);
            t *= glm::translate(t, T);

            vec3 R = vec3(node->rotation[0], node->rotation[1], node->rotation[2]);
            float theta = node->rotation[3];
            if (R.x || R.y || R.z) {
                t *= glm::rotate(t, theta, R);
            }

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
                for (uint32 i = 0; i < accessor.count; i++) {
                    indices[i] = *(uint8*)(&model->buffer()[nBytes + i * nSize]);
                }
            } else if (nSize == 2) {
                for (uint32 i = 0; i < accessor.count; i++) {
                    indices[i] = *(uint16*)(&model->buffer()[nBytes + i * nSize]);
                }
            } else if (nSize == 4) {
                for (uint32 i = 0; i < accessor.count; i++) {
                    indices[i] = *(uint32*)(&model->buffer()[nBytes + i * nSize]);
                }
            } else if (nSize == 8) {
                for (uint32 i = 0; i < accessor.count; i++) {
                    indices[i] = static_cast<uint32>(*(uint64*)(&model->buffer()[nBytes + i * nSize]));
                }
            }
        }

        CHECK(primitive.mode == glTF::TRIANGLES);

        std::vector<Vertex> vertices(positions.size());
        for (usize i = 0; i < vertices.size(); i++) {
            vertices[i].position = positions[i];

            if (i < normals.size()) {
                vertices[i].normal = normals[i];
            }

            if (i < texCoords.size()) {
                vertices[i].textureCoords = texCoords[i];
            }
        }

        m_prototypes.emplace_back(MeshPrototype{
            .vertexBuffer = resourceManager->allocateVertexBuffer({
                .physicalDevice = *m_physicalDevice,
                .logicalDevice = *m_logicalDevice,
                .commandBuffer = m_commandPool->commandBuffers().front(),
                .vertices = vertices,
            }),
            .indexBuffer = resourceManager->allocateIndexBuffer({
                .physicalDevice = *m_physicalDevice,
                .logicalDevice = *m_logicalDevice,
                .commandBuffer = m_commandPool->commandBuffers().front(),
                .indices = indices,
            }),
            .textureIndices = {},
        });

        if (primitive.material != glTF::UNDEFINED) {
            processMaterial(model, &model->materials[primitive.material]);
        }
    }
}

void ModelLoader::processMaterial(glTF::Model* model, glTF::Material* material) {
    int32 pbrSpecularGlossiness_ExtensionIndex = -1;

    for (int32 i = 0; auto& extension : material->extensions) {
        if (std::strcmp(extension->name, glTF::EXTENSION_KHR_materials_pbrSpecularGlossiness) == 0) {
            pbrSpecularGlossiness_ExtensionIndex = i;
        }
        i++;
    }

    if (material->emissiveTexture) {
        processTexture(model, &material->emissiveTexture.value(), TextureType::Emissive);
    }

    if (material->occlusionTexture) {
        processTexture(model, &material->occlusionTexture.value(), TextureType::AmbientOcclusion);
    }

    if (material->normalTexture) {
        processTexture(model, &material->normalTexture.value(), TextureType::Normal);
    }

    if (material->pbrMetallicRoughness.metallicRoughnessTexture) {
        processTexture(
            model, &material->pbrMetallicRoughness.metallicRoughnessTexture.value(), TextureType::MetallicRoughness);
    }

    if (material->pbrMetallicRoughness.baseColorTexture) {
        processTexture(model, &*material->pbrMetallicRoughness.baseColorTexture, TextureType::Albedo);
    } else if (pbrSpecularGlossiness_ExtensionIndex >= 0) {
        auto* ext = (glTF::KHR_materials_pbrSpecularGlossiness*)&*material
                        ->extensions[pbrSpecularGlossiness_ExtensionIndex]; processTexture(
                            model, &ext->diffuseTexture.value(), TextureType::Albedo);
    }
}

void ModelLoader::processTexture(glTF::Model* model, glTF::TextureInfo* textureInfo, TextureType type) {
    if (m_loadedTextures.contains(textureInfo->index)) {
        m_prototypes.back().textureIndices.emplace_back(m_loadedTextures[textureInfo->index]);
        return;
    } else {
        m_loadedTextures.emplace(textureInfo->index, m_textures.size());
    }

    glTF::Texture& texture = model->textures[textureInfo->index];

    if (texture.source != glTF::UNDEFINED) {
        m_prototypes.back().textureIndices.emplace_back(static_cast<uint32>(m_textures.size()));
        glTF::Image& image = model->images[texture.source];
        auto path = std::string(m_directory + image.uri);

        if (!image.uri.empty()) {
            m_textures.push_back(resourceManager->allocateTexture({
                .physicalDevice = *m_physicalDevice,
                .logicalDevice = *m_logicalDevice,
                .commandBuffer = m_commandPool->commandBuffers().front(),
                .path = path.c_str(),
                .type = type,
            }));
        } else {
            glTF::BufferView& bufferView = model->bufferViews[image.bufferView];
            const uint8* data = model->buffer().data() + bufferView.byteOffset;

            m_textures.push_back(resourceManager->allocateTexture({
                .physicalDevice = *m_physicalDevice,
                .logicalDevice = *m_logicalDevice,
                .commandBuffer = m_commandPool->commandBuffers().front(),
                .width = bufferView.byteLength,
                .height = 0,
                .data = data,
                .type = type,
            }));
        }
    }
}

void ModelLoader::processTexture(glTF::Model* model, glTF::NormalTextureInfo* textureInfo, TextureType type) {
    glTF::TextureInfo adapter{.index = textureInfo->index};
    processTexture(model, &adapter, type);
}

void ModelLoader::processTexture(glTF::Model* model, glTF::OcclusionTextureInfo* textureInfo, TextureType type) {
    glTF::TextureInfo adapter{.index = textureInfo->index};
    processTexture(model, &adapter, type);
}

} // namespace R3
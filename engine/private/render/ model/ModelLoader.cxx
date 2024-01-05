#include "render/model/ModelLoader.hpp"

#include <R3>
#include "core/Scene.hpp"
#include "media/glTF/glTF-Extensions.hxx"
#include "media/glTF/glTF-Model.hxx"
#include "private/render/ResourceManager.hxx"
#include "render/CommandPool.hpp"
#include "render/ResourceManager.hxx"
#include "render/ShaderObjects.hpp"

namespace R3 {

static ResourceManager* resourceManager;

namespace local {

static usize datatypeSize(uint32 datatype) {
    switch (datatype) {
        case glTF::UNSIGNED_BYTE:
        case glTF::BYTE:
            return sizeof(uint8);
        case glTF::UNSIGNED_SHORT:
        case glTF::SHORT:
            return sizeof(uint16);
        case glTF::UNSIGNED_INT:
            return sizeof(uint32);
        case glTF::FLOAT:
            return sizeof(float);
        default:
            ENSURE(false);
    }
};

} // namespace local

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
    processKeyFrames(&gltf);

    for (auto& prototype : m_prototypes) {
        Mesh mesh;

        mesh.vertexBuffer = prototype.vertexBuffer;
        mesh.indexBuffer = prototype.indexBuffer;

        // Descriptor Set Layout Bindings
        static const std::vector<DescriptorSetLayoutBinding> layoutBindings = {
            // { binding, type, count, stage }

            // Uniform Buffer Object
            {0, DescriptorType::UniformBuffer, 1, ShaderStage::Vertex},
            // Albedo
            {1, DescriptorType::CombinedImageSampler, 1, ShaderStage::Fragment},
            // Metallic Roughness
            {2, DescriptorType::CombinedImageSampler, 1, ShaderStage::Fragment},
            // Normal
            {3, DescriptorType::CombinedImageSampler, 1, ShaderStage::Fragment},
            // Ambient Occlusion
            {4, DescriptorType::CombinedImageSampler, 1, ShaderStage::Fragment},
            // Emissive
            {5, DescriptorType::CombinedImageSampler, 1, ShaderStage::Fragment},
            // Lighting
            {6, DescriptorType::UniformBuffer, 1, ShaderStage::Fragment},
            // MousePicker
            {7, DescriptorType::StorageBuffer, 1, ShaderStage::Fragment},
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

        for (auto i = 0; i < PBR_TEXTURE_COUNT; i++) {
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

        StorageDescriptor storageDescriptors[] = {{*m_storageBuffer, 7}};

        auto& descriptorPool = resourceManager->getDescriptorPoolById(mesh.material.descriptorPool);
        auto& descriptorSets = descriptorPool.descriptorSets();

        for (uint32 i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
            descriptorSets[i].bindResources({uniformDescriptors, storageDescriptors, textureDescriptors});
        }

        model.meshes.emplace_back(std::move(mesh));
    }

    model.nodes = std::move(m_nodes);
    model.keyFrames = std::move(m_keyFrames);
    model.skins = std::move(m_skins);

    // save for reuse
    m_nodes.clear();
    m_prototypes.clear();
    m_keyFrames.clear();
    m_skins.clear();
    m_textures.clear();
    m_loadedTextures.clear();
}

void ModelLoader::processNode(glTF::Model* model, glTF::Node* node) {
    // push a new node containing metadata (we copy to preserve data)
    // - skin       : index into skin
    // - mesh       : index into mesh
    // - weights    : weights of morph target ???
    // - children   : child nodes in hierarchy
    m_nodes.emplace_back(node->skin, node->mesh, node->weights, node->children);

    if (node->mesh != undefined) {
        processMesh(model, node, &model->meshes[node->mesh]);
    }

    if (node->skin != undefined) {
        processSkin(model, &model->skins[node->skin]);
    }

    for (auto child : node->children) {
        processNode(model, &model->nodes[child]);
    }
}

void ModelLoader::processMesh(glTF::Model* model, glTF::Node* node, glTF::Mesh* mesh) {
    auto populateVertexAttrib = [=]<typename T>(const auto& primitive, std::vector<T>& vec, const char* attrib) {
        if (primitive.attributes.HasMember(attrib)) {
            glTF::Accessor& accessor = model->accessors[primitive.attributes[attrib].GetUint()];
            glTF::BufferView& bufferView = model->bufferViews[accessor.bufferView];
            uint32 offset = accessor.byteOffset + bufferView.byteOffset;
            vec.resize(accessor.count);
            std::memcpy(vec.data(), &model->buffer()[offset], accessor.count * sizeof(T));
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
            mat4 t = glm::make_mat4(node->matrix);

            vec3 T = vec3(node->translation[0], node->translation[1], node->translation[2]);
            t = glm::translate(t, T);

            mat4 R = glm::mat4_cast(quat(node->rotation[3], node->rotation[0], node->rotation[1], node->rotation[2]));
            t = t * R;

            vec3 S = vec3(node->scale[0], node->scale[1], node->scale[2]);
            t = glm::scale(t, S);

            position = mat3(t) * position;
        }

        std::vector<vec3> normals;
        populateVertexAttrib(primitive, normals, glTF::NORMAL);

        std::vector<vec2> texCoords;
        populateVertexAttrib(primitive, texCoords, glTF::TEXCOORD_0);

        std::vector<uint32> indices;
        if (primitive.indices != undefined) {
            glTF::Accessor& accessor = model->accessors[primitive.indices];
            glTF::BufferView& bufferView = model->bufferViews[accessor.bufferView];

            indices.resize(accessor.count);

            uint32 offset = accessor.byteOffset + bufferView.byteOffset;

            auto nSize = local::datatypeSize(accessor.componentType);
            switch (nSize) {
                case sizeof(uint8):
                    std::generate_n(indices.begin(), accessor.count, [=, i = 0]() mutable -> uint32 {
                        return *reinterpret_cast<const uint8*>(&model->buffer()[offset + i++ * nSize]);
                    });
                    break;
                case sizeof(uint16):
                    std::generate_n(indices.begin(), accessor.count, [=, i = 0]() mutable -> uint32 {
                        return *reinterpret_cast<const uint16*>(&model->buffer()[offset + i++ * nSize]);
                    });
                    break;
                default:
                    std::generate_n(indices.begin(), accessor.count, [=, i = 0]() mutable -> uint32 {
                        return *reinterpret_cast<const uint32*>(&model->buffer()[offset + i++ * nSize]);
                    });
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

        if (primitive.material != undefined) {
            processMaterial(model, &model->materials[primitive.material]);
        }
    }
}

void ModelLoader::processSkin(glTF::Model* model, glTF::Skin* skin) {
    std::vector<mat4> inverseBindMatrices;

    if (skin->inverseBindMatrices != undefined) {
        glTF::Accessor& accessor = model->accessors[skin->inverseBindMatrices];
        glTF::BufferView& bufferView = model->bufferViews[accessor.bufferView];

        uint32 offset = accessor.byteOffset + bufferView.byteOffset;

        inverseBindMatrices.resize(accessor.count);
        std::memcpy(inverseBindMatrices.data(), &model->buffer()[offset], accessor.count * sizeof(mat4));
    }

    m_skins.emplace_back(std::move(inverseBindMatrices), skin->joints, skin->skeleton);
}

void ModelLoader::processKeyFrames(glTF::Model* model) {
    // copies data from accessor bufferView to vec of type T
    auto populateSamplerInOut = [&]<typename T>(usize accessorIndex, std::vector<T>& vec) {
        glTF::Accessor& accessor = model->accessors[accessorIndex];
        glTF::BufferView& bufferView = model->bufferViews[accessor.bufferView];
        uint32 offset = accessor.byteOffset + bufferView.byteOffset;

        vec.resize(accessor.count);
        std::memcpy(vec.data(), &model->buffer()[offset], accessor.count * sizeof(T));
    };

    for (glTF::Animation& animation : model->animations) {
        for (glTF::AnimationChannel& channel : animation.channels) {
            // which TRS is it
            KeyFrame::ModifierType modifierType = KeyFrame::stringToModifier(channel.target.path);

            glTF::AnimationSampler& sampler = animation.samplers[channel.sampler];

            // collect timestamp
            std::vector<float> inputTimestamps;
            populateSamplerInOut(sampler.input, inputTimestamps);

            // addKeyFrames by populating a vector of vec3 or quat
            auto addKeyFrames = [&]<typename T>(std::vector<T>& modifiers) {
                populateSamplerInOut(sampler.output, modifiers);

                for (usize i = 0; float timestamp : inputTimestamps) {
                    m_keyFrames.emplace_back(timestamp, channel.target.node, modifierType, modifiers[i++]);
                }
            };

            if (modifierType == KeyFrame::Translation) { // populate vec3 translate data
                std::vector<vec3> translations;
                addKeyFrames(translations);
            } else if (modifierType == KeyFrame::Rotation) { // populate quat rotation data
                std::vector<quat> rotations;
                addKeyFrames(rotations);
            } else if (modifierType == KeyFrame::Scale) { // populate vec3 scale data
                std::vector<vec3> scales;
                addKeyFrames(scales);
            }
        } // for (channel : animation.channels)
    }     // for (animation : model->animations)
}

void ModelLoader::processMaterial(glTF::Model* model, glTF::Material* material) {
    int32 pbrSpecularGlossiness_ExtensionIndex = undefined;

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
    } else if (pbrSpecularGlossiness_ExtensionIndex != undefined) {
        auto* ext =
            (glTF::KHR_materials_pbrSpecularGlossiness*)&*material->extensions[pbrSpecularGlossiness_ExtensionIndex];
        processTexture(model, &ext->diffuseTexture.value(), TextureType::Albedo);
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

    if (texture.source != undefined) {
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
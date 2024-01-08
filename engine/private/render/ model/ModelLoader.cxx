#include "render/model/ModelLoader.hpp"

#include <R3>
#include "core/Scene.hpp"
#include "media/glTF/glTF-Extensions.hxx"
#include "media/glTF/glTF-Model.hxx"
#include "render/CommandPool.hpp"
#include "render/ShaderObjects.hpp"

namespace R3 {

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
        case glTF::INT:
            return sizeof(uint32);
        case glTF::FLOAT:
            return sizeof(float);
        default:
            ENSURE(false);
    }
};

static usize componentElements(std::string_view component) {
    if (component == "SCALAR") {
        return 1;
    } else if (component == "VEC2") {
        return 2;
    } else if (component == "VEC3") {
        return 3;
    } else if (component == "VEC4") {
        return 4;
    } else if (component == "MAT2") {
        return 4;
    } else if (component == "MAT3") {
        return 9;
    } else if (component == "MAT4") {
        return 16;
    } else {
        ENSURE(false);
    }
};

template <typename T, typename N = T>
static void readAccessor(glTF::Model& model, usize accessorIndex, std::vector<T>& data) {
    CHECK(data.empty());

    glTF::Accessor& accessor = model.accessors[accessorIndex];
    glTF::BufferView& bufferView = model.bufferViews[accessor.bufferView];

    CHECK(datatypeSize(accessor.componentType) * componentElements(accessor.type) == sizeof(N));

    uint32 offset = accessor.byteOffset + bufferView.byteOffset;
    data.resize(accessor.count);

    // can take a vector of <typename T> and read the accessor as <typename N>
    // eg accesor<uint16>[] --> std::vector<uint32>
    usize i = 0;
    std::ranges::generate(data, [&] { return *std::bit_cast<const N*>(&model.buffer()[offset + i++ * sizeof(N)]); });
}

} // namespace local

ModelLoader::ModelLoader(const ModelLoaderSpecification& spec)
    : m_physicalDevice(&spec.physicalDevice),
      m_logicalDevice(&spec.logicalDevice),
      m_swapchain(&spec.swapchain),
      m_renderPass(&spec.renderPass),
      m_commandPool(&spec.commandPool),
      m_storageBuffer(&spec.storageBuffer) {
    const uint32 data = 0x00FF'FFFF; // forfills glTF spec of white base color on missing pbrMetallicRoughness
    const TextureBufferSpecification nilTextureSpec = {
        .physicalDevice = *m_physicalDevice,
        .logicalDevice = *m_logicalDevice,
        .commandBuffer = m_commandPool->commandBuffers().front(),
        .width = 1,
        .height = 1,
        .raw = std::bit_cast<const std::byte*>(&data),
        .type = TextureType::Nil,
    };
    m_nilTexture = std::make_shared<TextureBuffer>(nilTextureSpec);
}

void ModelLoader::load(const std::string& path, ModelComponent& model) {
    usize split = path.find_last_of('/') + 1;
    m_directory = path.substr(0, split);
    auto file = path.substr(split);

    glTF::Model gltf(m_directory + file);

    preProcessTextures(gltf);
    for (auto& scene : gltf.scenes) {
        for (uint32 iNode : scene.nodes) {
            processNode(gltf, gltf.nodes[iNode]);
        }
    }
    processAnimations(gltf);
    processSkeleton(gltf);

    model.meshes.reserve(m_prototypes.size());
    for (auto& prototype : m_prototypes) {
        Mesh mesh;

        mesh.vertexBuffer = std::move(prototype.vertexBuffer);
        mesh.indexBuffer = std::move(prototype.indexBuffer);

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
        mesh.material.descriptorPool = DescriptorPool({
            .logicalDevice = *m_logicalDevice,
            .descriptorSetCount = MAX_FRAMES_IN_FLIGHT,
            .layoutBindings = layoutBindings,
        });

        // Pipeline
        mesh.pipeline = GraphicsPipeline({
            .physicalDevice = *m_physicalDevice,
            .logicalDevice = *m_logicalDevice,
            .swapchain = *m_swapchain,
            .renderPass = *m_renderPass,
            .descriptorSetLayout = mesh.material.descriptorPool.layout(),
            .vertexBindingSpecification = Vertex::vertexBindingSpecification(),
            .vertexAttributeSpecification = Vertex::vertexAttributeSpecification(),
            .vertexShaderPath = "spirv/pbr.vert.spv",
            .fragmentShaderPath = "spirv/pbr.frag.spv",
            .msaa = true,
        });

        // Uniform
        for (auto i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
            mesh.material.uniforms[i] = UniformBuffer({
                .physicalDevice = *m_physicalDevice,
                .logicalDevice = *m_logicalDevice,
                .bufferSize = sizeof(VertexUniformBufferObject),
            });
            mesh.material.uniforms[i + 3] = UniformBuffer({
                .physicalDevice = *m_physicalDevice,
                .logicalDevice = *m_logicalDevice,
                .bufferSize = sizeof(FragmentUniformBufferObject),
            });
        }

        // Textures
        std::vector<TextureDescriptor> textureDescriptors;

        for (usize index : prototype.textureIndices) {
            TextureType type = m_textures[index]->type();
            mesh.material.pbrFlags |= (1 << (uint32(type) - 1));

            switch (type) {
                case TextureType::Albedo:
                    mesh.material.textures.albedo = m_textures[index];
                    textureDescriptors.emplace_back(*mesh.material.textures.albedo, 1);
                    break;
                case TextureType::MetallicRoughness:
                    mesh.material.textures.metallicRoughness = m_textures[index];
                    textureDescriptors.emplace_back(*mesh.material.textures.metallicRoughness, 2);
                    break;
                case TextureType::Normal:
                    mesh.material.textures.normal = m_textures[index];
                    textureDescriptors.emplace_back(*mesh.material.textures.normal, 3);
                    break;
                case TextureType::AmbientOcclusion:
                    mesh.material.textures.ambientOcclusion = m_textures[index];
                    textureDescriptors.emplace_back(*mesh.material.textures.ambientOcclusion, 4);
                    break;
                case TextureType::Emissive:
                    mesh.material.textures.emissive = m_textures[index];
                    textureDescriptors.emplace_back(*mesh.material.textures.emissive, 5);
                    break;
                default:
                    break;
            }
        }

        if (!(mesh.material.pbrFlags & TexturePBR::ALBEDO_FLAG)) {
            mesh.material.textures.albedo = m_nilTexture;
            textureDescriptors.emplace_back(*mesh.material.textures.albedo, 1);
        }
        if (!(mesh.material.pbrFlags & TexturePBR::METALLIC_ROUGHNESS_FLAG)) {
            mesh.material.textures.metallicRoughness = m_nilTexture;
            textureDescriptors.emplace_back(*mesh.material.textures.metallicRoughness, 2);
        }
        if (!(mesh.material.pbrFlags & TexturePBR::NORMAL_FLAG)) {
            mesh.material.textures.normal = m_nilTexture;
            textureDescriptors.emplace_back(*mesh.material.textures.normal, 3);
        }
        if (!(mesh.material.pbrFlags & TexturePBR::AMBIENT_OCCLUSION_FLAG)) {
            mesh.material.textures.ambientOcclusion = m_nilTexture;
            textureDescriptors.emplace_back(*mesh.material.textures.ambientOcclusion, 4);
        }
        if (!(mesh.material.pbrFlags & TexturePBR::EMISSIVE_FLAG)) {
            mesh.material.textures.emissive = m_nilTexture;
            textureDescriptors.emplace_back(*mesh.material.textures.emissive, 5);
        }

        // Bindings
        std::vector<UniformDescriptor> uniformDescriptors;
        for (uint32 i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
            uniformDescriptors.emplace_back(mesh.material.uniforms[i], 0);
            uniformDescriptors.emplace_back(mesh.material.uniforms[i + 3], 6);
        };

        StorageDescriptor storageDescriptors[] = {{*m_storageBuffer, 7}};

        DescriptorPool& descriptorPool = mesh.material.descriptorPool;
        std::vector<DescriptorSet>& descriptorSets = descriptorPool.descriptorSets();

        for (uint32 i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
            descriptorSets[i].bindResources({uniformDescriptors, storageDescriptors, textureDescriptors});
        }

        model.meshes.emplace_back(std::move(mesh));
    }

    model.animation.keyFrames = std::move(m_keyFrames);
    model.skeleton = std::move(m_skeleton);

    // save for reuse
    m_prototypes.clear();
    m_keyFrames.clear();
    m_textures.clear();
    m_skeleton = Skeleton();
}

void ModelLoader::processNode(glTF::Model& model, glTF::Node& node) {
    for (auto child : node.children) {
        processNode(model, model.nodes[child]);
    }

    if (node.mesh != undefined) {
        processMesh(model, model.meshes[node.mesh]);
    }
}

void ModelLoader::processMesh(glTF::Model& model, glTF::Mesh& mesh) {
    for (auto& primitive : mesh.primitives) {
        CHECK(primitive.mode == glTF::TRIANGLES);

        //--- Vertices
        std::vector<vec3> positions;
        CHECK(primitive.attributes.HasMember(glTF::POSITION));
        local::readAccessor(model, primitive.attributes[glTF::POSITION].GetUint(), positions);

        std::vector<vec3> normals;
        if (primitive.attributes.HasMember(glTF::NORMAL)) {
            local::readAccessor(model, primitive.attributes[glTF::NORMAL].GetUint(), normals);
        }

        std::vector<vec2> texCoords;
        if (primitive.attributes.HasMember(glTF::TEXCOORD_0)) {
            local::readAccessor(model, primitive.attributes[glTF::TEXCOORD_0].GetUint(), texCoords);
        }

        std::vector<ivec4> joints;
        if (primitive.attributes.HasMember(glTF::JOINTS_0)) {
            usize index = primitive.attributes[glTF::JOINTS_0].GetUint();
            std::vector<glm::vec<4, uint16>> jointIndices;

            if (local::datatypeSize(model.accessors[index].componentType) == sizeof(uint8)) {
                local::readAccessor<decltype(jointIndices)::value_type, glm::vec<4, uint8>>(model, index, jointIndices);
            } else if (local::datatypeSize(model.accessors[index].componentType) == sizeof(uint16)) {
                local::readAccessor(model, index, jointIndices);
            } else {
                LOG(Error, "unsupported joint datatype", model.accessors[index].componentType);
            }

            joints.resize(jointIndices.size());
            std::ranges::copy(jointIndices, joints.begin());
        }

        std::vector<vec4> weights;
        if (primitive.attributes.HasMember(glTF::WEIGHTS_0)) {
            usize index = primitive.attributes[glTF::WEIGHTS_0].GetUint();

            if (local::datatypeSize(model.accessors[index].componentType) == sizeof(uint8)) {
                local::readAccessor<vec4, glm::vec<4, uint8>>(model, index, weights);
            } else if (local::datatypeSize(model.accessors[index].componentType) == sizeof(uint16)) {
                local::readAccessor<vec4, glm::vec<4, uint16>>(model, index, weights);
            } else {
                local::readAccessor(model, index, weights);
            }
        }

        std::vector<Vertex> vertices(positions.size());
        for (usize i = 0; i < vertices.size(); i++) {
            vertices[i].position = positions[i];
            vertices[i].normal = i < normals.size() ? normals[i] : vec3(0);
            vertices[i].textureCoords = i < texCoords.size() ? texCoords[i] : vec2(0);
            vertices[i].boneIDs = i < joints.size() ? joints[i] : ivec4(-1);
            vertices[i].weights = i < weights.size() ? weights[i] : vec4(0.0f);
        }

        //--- Indices
        std::vector<uint32> indices;
        if (primitive.indices != undefined) {
            glTF::Accessor& accessor = model.accessors[primitive.indices];

            switch (local::datatypeSize(accessor.componentType)) {
                case sizeof(uint8):
                    local::readAccessor<uint32, uint8>(model, primitive.indices, indices);
                    break;
                case sizeof(uint16):
                    local::readAccessor<uint32, uint16>(model, primitive.indices, indices);
                    break;
                case sizeof(uint32):
                    local::readAccessor(model, primitive.indices, indices);
                    break;
                default:
                    LOG(Warning, "unknown datatype size");
            }
        } else {
            LOG(Verbose, "mesh does not contain indices");
        }

        m_prototypes.emplace_back(MeshPrototype{
            .vertexBuffer = VertexBuffer({
                .physicalDevice = *m_physicalDevice,
                .logicalDevice = *m_logicalDevice,
                .commandBuffer = m_commandPool->commandBuffers().front(),
                .vertices = vertices,
            }),
            .indexBuffer = IndexBuffer<uint32>({
                .physicalDevice = *m_physicalDevice,
                .logicalDevice = *m_logicalDevice,
                .commandBuffer = m_commandPool->commandBuffers().front(),
                .indices = indices,
            }),
            .textureIndices = {},
        });

        if (primitive.material != undefined) {
            processMaterial(model, model.materials[primitive.material]);
        }
    }
}

void ModelLoader::processAnimations(glTF::Model& model) {
    // for every channel of every animation, get the sampler timestamps
    // use those timestamps as key so that we can build up a mat4 that represents the TRS transform gotten from
    // the individual Translation, Rotation, and Scale components of the glTF file
    for (glTF::Animation& animation : model.animations) {
        for (glTF::AnimationChannel& channel : animation.channels) {
            KeyFrame::ModifierType modifierType = KeyFrame::stringToModifier(channel.target.path);

            glTF::AnimationSampler& sampler = animation.samplers[channel.sampler];

            // collect timestamps
            std::vector<float> inputTimestamps;
            local::readAccessor(model, sampler.input, inputTimestamps);

            // addKeyFrames by populating a vector of vec3 or quat
            auto addKeyFrames = [&]<typename T>(std::vector<T>& modifiers) {
                local::readAccessor(model, sampler.output, modifiers);

                for (usize i = 0; float timestamp : inputTimestamps) {
                    if constexpr (std::is_same_v<T, vec3>) {
                        m_keyFrames.emplace_back(
                            timestamp, channel.target.node, modifierType, vec4(modifiers[i], 0.0f));
                    } else {
                        m_keyFrames.emplace_back(timestamp,
                                                 channel.target.node,
                                                 modifierType,
                                                 vec4(modifiers[i].x, modifiers[i].y, modifiers[i].z, modifiers[i].w));
                    }

                    i++;
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
            } else if (modifierType == KeyFrame::Weights) { // populate weights or something
                LOG(Warning, "weights not yet supported");
            }
        } // for (channel : animation.channels)
    }     // for (animation : model.animations)
}

void ModelLoader::processSkeleton(glTF::Model& model) {
    usize numberOfSkeletons = model.skins.size();

    if (numberOfSkeletons == 0) {
        return;
    }

    if (numberOfSkeletons > 1) {
        LOG(Warning, "more that one skeleton");
    }

    const auto& skin = model.skins[0];

    if (skin.inverseBindMatrices != undefined) {
        usize numberOfJoints = skin.joints.size();

        m_skeleton.joints.resize(numberOfJoints);
        m_skeleton.finalJointsMatrices.resize(numberOfJoints);

        LOG(Verbose, "loading skeleton:", !skin.name.empty() ? skin.name : "UNNAMED");

        std::vector<mat4> inverseBindMatrices;
        local::readAccessor(model, skin.inverseBindMatrices, inverseBindMatrices);

        for (usize i = 0; i < numberOfJoints; i++) {
            auto rootIndex = skin.joints[i];

            m_skeleton.joints[i].rootIndex = rootIndex;
            m_skeleton.joints[i].inverseBindMatrix = inverseBindMatrices[i];

            auto& node = model.nodes[rootIndex];

            m_skeleton.joints[i].deformedTranslation = glm::make_vec3(node.translation);
            m_skeleton.joints[i].deformedRotation = glm::mat4(glm::make_quat(node.rotation));
            m_skeleton.joints[i].deformedScale = glm::make_vec3(node.scale);
            m_skeleton.joints[i].undeformedMatrix = glm::make_mat4(node.matrix);

            m_skeleton.nodeToJointMap.emplace(rootIndex, i);
        }

        usize rootJoint = skin.joints[0];

        processJoint(model, rootJoint, undefined);
    }
}

void ModelLoader::processJoint(glTF::Model& model, usize rootIndex, usize parentJoint) {
    usize currentJoint = m_skeleton.nodeToJointMap[rootIndex];

    auto& joint = m_skeleton.joints[currentJoint];

    joint.parentJoint = parentJoint;

    usize numberOfChildren = model.nodes[rootIndex].children.size();
    if (numberOfChildren > 0) {
        joint.children.resize(numberOfChildren);

        for (auto i = 0; i < numberOfChildren; i++) {
            usize childRootIndex = model.nodes[rootIndex].children[i];
            joint.children[i] = m_skeleton.nodeToJointMap[childRootIndex];
            processJoint(model, childRootIndex, currentJoint);
        }
    }
}

void ModelLoader::processMaterial(glTF::Model& model, glTF::Material& material) {
    int32 pbrSpecularGlossiness_INDEX = undefined;

    for (int32 i = 0; auto& extension : material.extensions) {
        if (std::strcmp(extension->name, glTF::EXTENSION_KHR_materials_pbrSpecularGlossiness) == 0) {
            pbrSpecularGlossiness_INDEX = i;
        }
        i++;
    }

    if (material.emissiveTexture) {
        processTexture(model, *material.emissiveTexture, TextureType::Emissive);
    }

    if (material.occlusionTexture) {
        processTexture(model, *material.occlusionTexture, TextureType::AmbientOcclusion);
    }

    if (material.normalTexture) {
        processTexture(model, *material.normalTexture, TextureType::Normal);
    }

    if (material.pbrMetallicRoughness.metallicRoughnessTexture) {
        processTexture(model, *material.pbrMetallicRoughness.metallicRoughnessTexture, TextureType::MetallicRoughness);
    }

    if (material.pbrMetallicRoughness.baseColorTexture) {
        processTexture(model, *material.pbrMetallicRoughness.baseColorTexture, TextureType::Albedo);
    } else if (pbrSpecularGlossiness_INDEX != undefined) {
        auto* ext = (glTF::KHR_materials_pbrSpecularGlossiness*)material.extensions[pbrSpecularGlossiness_INDEX].get();
        processTexture(model, *ext->diffuseTexture, TextureType::Albedo);
    } else {
        uint8 color[4] = {
            static_cast<uint8>(material.pbrMetallicRoughness.baseColorFactor[0] * 255.0f),
            static_cast<uint8>(material.pbrMetallicRoughness.baseColorFactor[1] * 255.0f),
            static_cast<uint8>(material.pbrMetallicRoughness.baseColorFactor[2] * 255.0f),
            static_cast<uint8>(material.pbrMetallicRoughness.baseColorFactor[3] * 255.0f),
        };
        processTexture(model, color, TextureType::Albedo);
    }
}

void ModelLoader::processTexture(glTF::Model&, uint8 color[4], TextureType type) {
    m_prototypes.back().textureIndices.emplace_back(static_cast<uint32>(m_textures.size()));

    TextureBufferSpecification textureBufferSpecification = {
        .physicalDevice = *m_physicalDevice,
        .logicalDevice = *m_logicalDevice,
        .commandBuffer = m_commandPool->commandBuffers().front(),
        .width = 1,
        .height = 1,
        .raw = std::bit_cast<const std::byte*>(color),
        .type = type,
    };

    m_textures.emplace_back(std::make_shared<TextureBuffer>(textureBufferSpecification));
}

void ModelLoader::processTexture(glTF::Model& model, glTF::TextureInfo& textureInfo, TextureType type) {
    glTF::Texture& texture = model.textures[textureInfo.index];

    if (texture.source != undefined) {
        m_prototypes.back().textureIndices.emplace_back(textureInfo.index);
        m_textures[textureInfo.index]->setType(type);
    }
}

void ModelLoader::processTexture(glTF::Model& model, glTF::NormalTextureInfo& textureInfo, TextureType type) {
    glTF::TextureInfo adapter{.index = textureInfo.index};
    processTexture(model, adapter, type);
}

void ModelLoader::processTexture(glTF::Model& model, glTF::OcclusionTextureInfo& textureInfo, TextureType type) {
    glTF::TextureInfo adapter{.index = textureInfo.index};
    processTexture(model, adapter, type);
}

void ModelLoader::preProcessTextures(glTF::Model& model) {
    m_textures.resize(model.textures.size());

    std::vector<std::thread> pool;
    pool.reserve(model.textures.size());

    for (uint32 i = 0; const glTF::Texture& texture : model.textures) {
        if (texture.source == uint32(undefined)) {
            m_textures.emplace_back(nullptr);
            continue;
        }

        glTF::Image& image = model.images[texture.source];
        std::string path = m_directory + image.uri;

        pool.emplace_back([&, i, path]() {
            CommandPool commandPool = CommandPoolSpecification{
                .logicalDevice = *m_logicalDevice,
                .swapchain = *m_swapchain,
                .type = CommandPoolType::Transient | CommandPoolType::Reset,
                .commandBufferCount = 1,
            };

            std::vector<CommandBuffer> commandBuffers = CommandBuffer::allocate({
                .logicalDevice = *m_logicalDevice,
                .swapchain = *m_swapchain,
                .commandPool = commandPool,
                .commandBufferCount = 1,
            });

            if (!image.uri.empty()) {
                TextureBufferSpecification textureBufferSpecification = {
                    .physicalDevice = *m_physicalDevice,
                    .logicalDevice = *m_logicalDevice,
                    .commandBuffer = commandBuffers.front(),
                    .path = path.c_str(),
                    .type = TextureType::Nil, // set later
                };
                m_textures[i] = std::make_shared<TextureBuffer>(textureBufferSpecification);
            } else {
                glTF::BufferView& bufferView = model.bufferViews[image.bufferView];
                const std::byte* data = std::bit_cast<const std::byte*>(&model.buffer()[bufferView.byteOffset]);

                TextureBufferSpecification textureBufferSpecification = {
                    .physicalDevice = *m_physicalDevice,
                    .logicalDevice = *m_logicalDevice,
                    .commandBuffer = commandBuffers.front(),
                    .width = bufferView.byteLength,
                    .height = 0,
                    .data = data,
                    .type = TextureType::Nil, // set later
                };
                m_textures[i] = std::make_shared<TextureBuffer>(textureBufferSpecification);
            }
        });

        i++;
    }

    for (auto& thread : pool) {
        thread.join();
    }
}

} // namespace R3
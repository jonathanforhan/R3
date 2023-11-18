#include "ModelComponent.hpp"
#include "detail/media/GLTF_Model.hxx"
#include "api/Check.hpp"
#include "api/Log.hpp"

namespace R3 {

ModelComponent::ModelComponent(const std::string& path, Shader& shader)
    : m_shader(shader),
      m_directory(),
      m_file() {
    usize split = path.find_last_of('/') + 1;
    m_directory = path.substr(0, split);
    m_file = path.substr(split);

    GLTF_Model gltf(path);

    for (auto& scene : gltf.scenes) {
        for (uint32 iNode : scene.nodes) {
            processNode(&gltf, &gltf.nodes[iNode]);
        }
    }

    m_mesh = {{m_vertices}, {m_indices}};
}

void ModelComponent::processNode(GLTF_Model* model, GLTF_Node* node) {
    for (uint32 child : node->children) {
        processNode(model, &model->nodes[child]);
    }

    if (node->mesh != GLTF_UNDEFINED) {
        processMesh(model, &model->meshes[node->mesh]);
    }
}

void ModelComponent::processMesh(GLTF_Model* model, GLTF_Mesh* mesh) {
    for (auto& primitive : mesh->primitives) {
        std::vector<float> positions;
        if (primitive.attributes.HasMember(GLTF_POSITION)) {
            uint32 iAccessor = primitive.attributes[GLTF_POSITION].GetUint();
            GLTF_Accessor& accessor = model->accessors[iAccessor];
            CHECK(accessor.type == GLTF_VEC3 && accessor.componentType == GLTF_FLOAT);

            GLTF_BufferView& bufferView = model->bufferViews[accessor.bufferView];
            GLTF_Buffer& buffer = model->buffers[bufferView.buffer];

            usize nBytes = (usize)accessor.byteOffset + bufferView.byteOffset;
            positions.resize(accessor.count * 3);
            memcpy(positions.data(), model->buffer().data() + nBytes, accessor.count * sizeof(float) * 3);
        }
        
        #if 0
        std::vector<float> normals;
        if (primitive.attributes.HasMember(GLTF_NORMAL)) {
            uint32 iAccessor = primitive.attributes[GLTF_NORMAL].GetUint();
            GLTF_Accessor& accessor = model->accessors[iAccessor];
            CHECK(accessor.type == GLTF_VEC3 && accessor.componentType == GLTF_FLOAT);

            GLTF_BufferView& bufferView = model->bufferViews[accessor.bufferView];
            GLTF_Buffer& buffer = model->buffers[bufferView.buffer];

            usize nBytes = (usize)accessor.byteOffset + bufferView.byteOffset;
            normals.resize(accessor.count);
            memcpy(normals.data(), model->buffer().data() + nBytes, accessor.count * sizeof(float));
        }

        std::vector<float> tangents;
        if (primitive.attributes.HasMember(GLTF_TANGENT)) {
            uint32 iAccessor = primitive.attributes[GLTF_TANGENT].GetUint();
            GLTF_Accessor& accessor = model->accessors[iAccessor];
            CHECK(accessor.type == GLTF_VEC3 && accessor.componentType == GLTF_FLOAT);

            GLTF_BufferView& bufferView = model->bufferViews[accessor.bufferView];
            GLTF_Buffer& buffer = model->buffers[bufferView.buffer];

            usize nBytes = (usize)accessor.byteOffset + bufferView.byteOffset;
            tangents.resize(accessor.count);
            memcpy(tangents.data(), model->buffer().data() + nBytes, accessor.count * sizeof(float));
        }
        #endif

        std::vector<uint16> indices;
        if (primitive.indices != GLTF_UNDEFINED) {
            GLTF_Accessor& accessor = model->accessors[primitive.indices];
            CHECK(accessor.type == GLTF_SCALAR && accessor.componentType == GLTF_UNSIGNED_SHORT);

            GLTF_BufferView& bufferView = model->bufferViews[accessor.bufferView];
            GLTF_Buffer& buffer = model->buffers[bufferView.buffer];

            usize nBytes = (usize)accessor.byteOffset + bufferView.byteOffset;
            indices.resize(accessor.count);
            memcpy(indices.data(), model->buffer().data() + nBytes, accessor.count * sizeof(uint16));
        }

        if (primitive.material != GLTF_UNDEFINED) {
        }

        CHECK(primitive.mode == GLTF_TRIANGLES);

        std::vector<Vertex> vertices(positions.size() / 3);
        for (usize i = 0; auto& vertex : vertices) {
            vertex.position.x = positions[i + 0];
            vertex.position.y = positions[i + 1];
            vertex.position.z = positions[i + 2];

            #if 0
            if (normals.size()) {
                vertex.normal.x = normals[i + 0];
                vertex.normal.y = normals[i + 1];
                vertex.normal.z = normals[i + 2];
            }

            if (tangents.size()) {
                vertex.tangent.x = tangents[i + 0];
                vertex.tangent.y = tangents[i + 1];
                vertex.tangent.z = tangents[i + 2];
            }
            #endif

            i += 3;
        }
        m_vertices.insert(m_vertices.end(), vertices.begin(), vertices.end());

        for (uint32 index : indices) {
            m_indices.push_back(index);
        }
    }
}
 

} // namespace R3
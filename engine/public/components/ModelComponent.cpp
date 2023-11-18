#include "ModelComponent.hpp"
#include "detail/media/GLTF_Model.hxx"
#include "api/Check.hpp"
#include "api/Ensure.hpp"
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
    auto datatypeSize = [](uint32 datatype) -> usize {
        if (datatype == GLTF_UNSIGNED_BYTE)
            return sizeof(uint8);
        if (datatype == GLTF_UNSIGNED_SHORT)
            return sizeof(uint16);
        if (datatype == GLTF_UNSIGNED_INT)
            return sizeof(uint32);
        if (datatype == GLTF_FLOAT)
            return sizeof(float);
        else
            ENSURE(false);
    };

    auto populateVertexAttrib = [=](const GLTF_MeshPrimitive& primitive, std::vector<float>& vec, const char* attrib) {
        if (primitive.attributes.HasMember(attrib)) {
            GLTF_Accessor& accessor = model->accessors[primitive.attributes[attrib].GetUint()];
            GLTF_BufferView& bufferView = model->bufferViews[accessor.bufferView];
            GLTF_Buffer& buffer = model->buffers[bufferView.buffer];

            constexpr uint32 nComponents = 3;
            usize nBytes = usize(accessor.byteOffset) + bufferView.byteOffset;
            usize nSize = datatypeSize(accessor.componentType);
            vec.resize((usize)accessor.count * nComponents);
            memcpy(vec.data(), model->buffer().data() + nBytes, accessor.count * nSize * nComponents);
        }
    };

    for (auto& primitive : mesh->primitives) {
        std::vector<float> positions;
        populateVertexAttrib(primitive, positions, GLTF_POSITION);

        std::vector<float> normals;
        populateVertexAttrib(primitive, normals, GLTF_NORMAL);

        std::vector<float> tangents;
        populateVertexAttrib(primitive, tangents, GLTF_TANGENT);

        std::vector<uint32> indices;
        if (primitive.indices != GLTF_UNDEFINED) {
            GLTF_Accessor& accessor = model->accessors[primitive.indices];
            GLTF_BufferView& bufferView = model->bufferViews[accessor.bufferView];
            GLTF_Buffer& buffer = model->buffers[bufferView.buffer];

            usize nBytes = (usize)accessor.byteOffset + bufferView.byteOffset;
            usize nSize = datatypeSize(accessor.componentType);
            indices.resize(accessor.count);
            if (nSize == 2) {
                for (uint32 i = 0; i < accessor.count; i++)
                    indices[i] = *(uint16*)(&model->buffer()[nBytes + i * nSize]);
            } else if (nSize == 4) {
                for (uint32 i = 0; i < accessor.count; i++)
                    indices[i] = *(uint32*)(&model->buffer()[nBytes + i * nSize]);
            } else {
                LOG(Warning, "exotic indice size, undefined behavior");
            }
        }

        if (primitive.material != GLTF_UNDEFINED) {
        }

        CHECK(primitive.mode == GLTF_TRIANGLES);

        std::vector<Vertex> vertices(positions.size() / 3);
        for (usize i = 0; auto& vertex : vertices) {
            vertex.position.x = positions[i + 0];
            vertex.position.y = positions[i + 1];
            vertex.position.z = positions[i + 2];

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

            i += 3;
        }
        m_vertices.insert(m_vertices.end(), vertices.begin(), vertices.end());

        for (uint32 index : indices) {
            m_indices.push_back(index);
        }
    }
}
 

} // namespace R3
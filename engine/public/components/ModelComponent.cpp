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

    auto populateVertexAttrib = [=](const GLTF_MeshPrimitive& primitive, auto& vec, const char* attrib) {
        if (primitive.attributes.HasMember(attrib)) {
            GLTF_Accessor& accessor = model->accessors[primitive.attributes[attrib].GetUint()];
            GLTF_BufferView& bufferView = model->bufferViews[accessor.bufferView];
            GLTF_Buffer& buffer = model->buffers[bufferView.buffer];

            uint32 nComponents = accessor.type == GLTF_VEC3 ? 3 : 2;
            usize nBytes = usize(accessor.byteOffset) + bufferView.byteOffset;
            usize nSize = datatypeSize(accessor.componentType);
            vec.resize((usize)accessor.count);
            memcpy(vec.data(), model->buffer().data() + nBytes, accessor.count * nSize * nComponents);
        }
    };

    for (auto& primitive : mesh->primitives) {
        std::vector<vec3> positions;
        populateVertexAttrib(primitive, positions, GLTF_POSITION);

        std::vector<vec3> normals;
        populateVertexAttrib(primitive, normals, GLTF_NORMAL);

        std::vector<vec3> tangents;
        populateVertexAttrib(primitive, tangents, GLTF_TANGENT);

        std::vector<vec2> texCoords;
        populateVertexAttrib(primitive, texCoords, GLTF_TEXCOORD_0);

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

        std::vector<Vertex> vertices(positions.size());
        for (usize i = 0; i < vertices.size(); i++) {
            vertices[i].position = positions[i];

            if (i < normals.size())
                vertices[i].normal = normals[i];

            if (i < tangents.size())
                vertices[i].tangent = tangents[i];

            if (i < texCoords.size())
                vertices[i].textureCoords = texCoords[i];
        }
        m_vertices.insert(m_vertices.end(), vertices.begin(), vertices.end());

        for (uint32 index : indices) {
            m_indices.push_back(index);
        }
    }
}
 

} // namespace R3
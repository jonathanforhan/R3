#include "ModelComponent.hpp"
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include "api/Check.hpp"
#include "api/Ensure.hpp"
#include "api/Log.hpp"
#include "core/Engine.hpp"

// clang-format off
constexpr auto ASSIMP_LOAD_FLAGS = (
    aiProcess_Triangulate |
    aiProcess_JoinIdenticalVertices |
    aiProcess_GenUVCoords |
    aiProcess_SortByPType |
    aiProcess_RemoveRedundantMaterials |
    aiProcess_FindInvalidData |
    aiProcess_CalcTangentSpace |
    aiProcess_GenSmoothNormals |
    aiProcess_ImproveCacheLocality |
    aiProcess_OptimizeMeshes |
    aiProcess_SplitLargeMeshes
);
// clang-format on

namespace R3 {

ModelComponent::ModelComponent(const std::string& directory, std::string_view file, Shader& shader, bool flipUVs)
    : m_shader(shader),
      m_directory(directory + "/") {
    Assimp::Importer importer;

    std::string path = "assets/" + m_directory + file.data();
    const aiScene* scene = importer.ReadFile(path.c_str(), ASSIMP_LOAD_FLAGS | (aiProcess_FlipUVs * flipUVs));

    bool isValidScene = scene && !(scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE) && scene->mRootNode;
    if (!isValidScene) {
        LOG(Error, "Assimp import error", importer.GetErrorString());
        ENSURE(isValidScene);
    }

    m_textures.reserve(scene->mNumMaterials);

    processNode(scene->mRootNode, scene);

    m_mesh = {{m_vertices.data(), m_vertices.size()}, {m_indices.data(), m_indices.size()}};
    m_vertices = {};
    m_indices = {};
}

void ModelComponent::processNode(aiNode* node, const aiScene* scene) {
    for (uint32 i = 0; i < node->mNumMeshes; i++) {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        processMesh(mesh, scene);
    }

    for (uint32 i = 0; i < node->mNumChildren; i++) {
        processNode(node->mChildren[i], scene);
    }
}

void ModelComponent::processMesh(aiMesh* mesh, const aiScene* scene) {
    std::vector<Vertex> vertices;
    vertices.reserve(mesh->mNumVertices);

    std::vector<uint32> indices;
    indices.reserve((size_t)mesh->mNumFaces * 3);

    for (uint32 i = 0; i < mesh->mNumVertices; i++) {
        Vertex vertex{};

        vertex.position.x = mesh->mVertices[i].x;
        vertex.position.y = mesh->mVertices[i].y;
        vertex.position.z = mesh->mVertices[i].z;

        if (mesh->HasNormals()) {
            vertex.normal.x = mesh->mNormals[i].x;
            vertex.normal.y = mesh->mNormals[i].y;
            vertex.normal.z = mesh->mNormals[i].z;
        }

        if (mesh->mTextureCoords[0]) {
            vertex.textureCoords.x = mesh->mTextureCoords[0][i].x;
            vertex.textureCoords.y = mesh->mTextureCoords[0][i].y;

            vertex.tangent.x = mesh->mTangents[i].x;
            vertex.tangent.y = mesh->mTangents[i].y;
            vertex.tangent.z = mesh->mTangents[i].z;

            vertex.bitangent.x = mesh->mBitangents[i].x;
            vertex.bitangent.y = mesh->mBitangents[i].y;
            vertex.bitangent.z = mesh->mBitangents[i].z;
        }

        vertices.push_back(vertex);
    }

    for (uint32 i = 0; i < mesh->mNumFaces; i++) {
        const aiFace& face = mesh->mFaces[i];
        CHECK(face.mNumIndices == 3);
        indices.push_back(face.mIndices[0]);
        indices.push_back(face.mIndices[1]);
        indices.push_back(face.mIndices[2]);
    }

    for (uint32 index : indices) {
        m_indices.push_back(index + static_cast<uint32>(m_vertices.size()));
    }
    for (Vertex& vertex : vertices) {
        m_vertices.push_back(vertex);
    }
    // m_vertices.insert(m_vertices.end(), vertices.begin(), vertices.end());

    if (mesh->mMaterialIndex >= 0 && m_loadedTextures.size() < scene->mNumMaterials) {
        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
        loadMaterialTextures(scene, material, aiTextureType_EMISSIVE, TextureType::Emissive);
        loadMaterialTextures(scene, material, aiTextureType_HEIGHT, TextureType::Height);
        loadMaterialTextures(scene, material, aiTextureType_NORMALS, TextureType::Normals);
        loadMaterialTextures(scene, material, aiTextureType_SPECULAR, TextureType::Specular);
        loadMaterialTextures(scene, material, aiTextureType_DIFFUSE, TextureType::Diffuse);
    }
}

void ModelComponent::loadMaterialTextures(const aiScene* scene, aiMaterial* material, uint32 typeFlag, TextureType type) {
    aiTextureType aiType = aiTextureType(typeFlag);
    aiString path;

    for (uint32 i = 0; i < material->GetTextureCount(aiType); i++) {
        ENSURE(material->GetTexture(aiType, 0, &path) == AI_SUCCESS);

        if (m_loadedTextures.contains(path.C_Str())) {
            continue;
        } else {
            m_loadedTextures.emplace(path.C_Str());
        }

        const aiTexture* texture = scene->GetEmbeddedTexture(path.C_Str());
        LOG(Info, path.C_Str(), texture);

        if (texture) {
            m_textures.emplace_back(texture->mWidth, texture->mHeight, (void*)texture->pcData, type);
        } else {
            m_textures.emplace_back("assets/" + m_directory + path.C_Str(), type);
        }
    }
}

} // namespace R3
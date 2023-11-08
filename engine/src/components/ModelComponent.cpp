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

    m_meshes.reserve(scene->mNumMeshes);
    m_textures.reserve(scene->mNumMaterials);

    processNode(scene->mRootNode, scene);
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
    std::vector<uint32> indices;

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
        } else {
            vertex.textureCoords = vec2(0.0f, 0.0f);
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

    m_meshes.emplace_back(std::span<Vertex>(vertices.data(), vertices.size()), std::span<uint32>(indices.data(), indices.size()));

    if (mesh->mMaterialIndex >= 0 && m_loadedTextures.size() < scene->mNumMaterials) {
        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
        loadMaterialTextures(material, aiTextureType_SPECULAR, TextureType::Specular);
        loadMaterialTextures(material, aiTextureType_NORMALS, TextureType::Normals);
        loadMaterialTextures(material, aiTextureType_HEIGHT, TextureType::Diffuse);
        loadMaterialTextures(material, aiTextureType_EMISSIVE, TextureType::Emissive);
        loadMaterialTextures(material, aiTextureType_DIFFUSE, TextureType::Diffuse);
    }
}

void ModelComponent::loadMaterialTextures(aiMaterial* material, uint32 typeFlag, TextureType type) {
    aiTextureType aiType = aiTextureType(typeFlag);

    for (uint32 i = 0; i < material->GetTextureCount(aiType); i++) {
        aiString str;
        ENSURE(material->GetTexture(aiType, 0, &str) == AI_SUCCESS);

        if (!m_loadedTextures.contains(str.C_Str())) {
            m_loadedTextures.emplace(str.C_Str());
            m_textures.emplace_back("assets/" + m_directory + str.C_Str(), type);
        }
    }
}

} // namespace R3
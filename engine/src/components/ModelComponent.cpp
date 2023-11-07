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
    aiProcess_FlipUVs |
    aiProcess_CalcTangentSpace |
    aiProcess_GenSmoothNormals |
    aiProcess_ImproveCacheLocality |
    aiProcess_OptimizeMeshes |
    aiProcess_SplitLargeMeshes
);
// clang-format on

namespace R3 {

Model::Model(std::string_view path, Shader& shader)
    : m_shader(shader) {
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(path.data(), ASSIMP_LOAD_FLAGS);
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode || scene->mNumMeshes == 0) {
        LOG(Error, "Assimp import error", importer.GetErrorString());
        ENSURE(NULL);
    }

    m_meshes.reserve(scene->mNumMeshes);
    m_textures.reserve(scene->mNumMaterials);

    processNode(scene->mRootNode, scene);
}

void Model::processNode(aiNode* node, const aiScene* scene) {
    for (uint32 i = 0; i < scene->mNumMeshes; i++) {
        aiMesh* mesh = scene->mMeshes[i];
        processMesh(mesh, scene);
    }

#if 0
    for (uint32 i = 0; i < node->mNumChildren; i++) {
        processNode(node->mChildren[i], scene);
    }
#else
    for (uint32 i = 0; i < scene->mNumMaterials; i++) {
        try {
            aiMaterial* material = scene->mMaterials[i];
            if (material->GetTextureCount(aiTextureType_DIFFUSE) > 0) {
                loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
            }
        } catch (std::exception& e) {
            LOG(Error, e.what());
        }
    }
#endif
}

void Model::processMesh(aiMesh* mesh, const aiScene* scene) {
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

#if 0
    if (mesh->mMaterialIndex < 0)
        return;

    aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

    try {
        loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
        // loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
        // loadMaterialTextures(material, aiTextureType_NORMALS, "texture_normal");
        // loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_height");
    } catch (std::exception& e) {
        LOG(Error, e.what());
    }
#endif
}

void Model::loadMaterialTextures(aiMaterial* material, uint32 typeFlag, std::string_view typeName) {
    static std::set<std::string> loadedTextures;
    aiTextureType type = aiTextureType(typeFlag);

    LOG(Info, "Material Count", material->GetTextureCount(type));

    for (uint32 i = 0; i < material->GetTextureCount(type); i++) {
        aiString s;
        ENSURE(material->GetTexture(type, 0, &s, 0, 0, 0, 0, 0) == AI_SUCCESS);

        if (!loadedTextures.contains(s.C_Str())) {
            loadedTextures.emplace(s.C_Str());
            LOG(Info, s.C_Str());
            m_textures.emplace_back(std::string("assets/backpack/") + s.C_Str());
        }
    }

    loadedTextures = {};
}

} // namespace R3
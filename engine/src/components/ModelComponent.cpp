#include "ModelComponent.hpp"
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <rapidjson/rapidjson.h>
#include <rapidjson/document.h>
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
      m_directory(directory + "/"),
      m_file(file) {
    Assimp::Importer importer;
    // rapidjson::Document document;
    // document.Parse();

    std::string path = "assets/" + m_directory + file.data();
    const aiScene* scene = importer.ReadFile(path.c_str(), ASSIMP_LOAD_FLAGS | (aiProcess_FlipUVs * flipUVs));

    bool isValidScene = scene && !(scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE) && scene->mRootNode;
    if (!isValidScene) {
        LOG(Error, "Assimp import error", importer.GetErrorString());
        ENSURE(isValidScene);
    }

    m_textures.reserve(scene->mNumMaterials);

    processNode(scene->mRootNode, scene);

    m_mesh = {{m_vertices}, {m_indices}};
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
    constexpr auto toGlmVec3 = [](const auto& v) -> glm::vec3 { return {v.x, v.y, v.z}; };
    constexpr auto toGlmVec2 = [](const auto& v) -> glm::vec2 { return {v.x, v.y}; };

    constexpr uint32 VERTICES_PER_TRIANGLE = 3;
    const uint32 vertexOffset = static_cast<uint32>(m_vertices.size());

    //--- populate index data
    for (uint32 i = 0; i < mesh->mNumFaces; i++) {
        const aiFace& face = mesh->mFaces[i];
        CHECK(face.mNumIndices == VERTICES_PER_TRIANGLE);

        m_indices.push_back(face.mIndices[0] + vertexOffset);
        m_indices.push_back(face.mIndices[1] + vertexOffset);
        m_indices.push_back(face.mIndices[2] + vertexOffset);
    }

    //--- populate vertex data
    for (uint32 i = 0; i < mesh->mNumVertices; i++) {
        Vertex vertex{};

        vertex.position = toGlmVec3(mesh->mVertices[i]);

        if (mesh->HasNormals()) {
            vertex.normal = toGlmVec3(mesh->mNormals[i]);
        }

        if (mesh->mTextureCoords[0]) {
            vertex.textureCoords = toGlmVec2(mesh->mTextureCoords[0][i]);
            vertex.tangent = toGlmVec3(mesh->mTangents[i]);
            vertex.bitangent = toGlmVec3(mesh->mBitangents[i]);
        }

        m_vertices.push_back(vertex);
    }

    //--- populate texture data
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

    for (uint32 i = 0; i < material->GetTextureCount(aiType); i++) {
        aiString path;
        ENSURE(material->GetTexture(aiType, 0, &path) == AI_SUCCESS);

        if (!m_loadedTextures.contains(path.C_Str())) {
            m_loadedTextures.emplace(path.C_Str());

            const aiTexture* embedded = scene->GetEmbeddedTexture(path.C_Str());

            embedded != nullptr
                ? m_textures.emplace_back(embedded->mWidth, embedded->mHeight, (void*)embedded->pcData, type)
                : m_textures.emplace_back("assets/" + m_directory + path.C_Str(), type);
        }
    }
}

} // namespace R3
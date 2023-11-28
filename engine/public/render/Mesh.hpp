#pragma once
#include <span>
#include <vector>
#include "api/Types.hpp"
#include "render/Vertex.hpp"

namespace R3 {

struct MeshSpecification {
    uint32 location;
    uint32 binding;
    uint32 offset;
};

class Mesh {
public:
    Mesh() = default; ///< @brief default constructable

    Mesh(std::span<Vertex> vertices, std::span<uint32> indices = {});

    Mesh(const Mesh&) = delete;           ///< @brief non-copyable
    void operator=(const Mesh&) = delete; ///< @brief non-copyable

    /// @brief Move constructor
    /// @param src
    Mesh(Mesh&& src) noexcept {
        this->~Mesh();
        m_vao = src.m_vao;
        m_vbo = src.m_vbo;
        m_ebo = src.m_ebo;
        m_vertexCount = src.m_vertexCount;
        m_indexCount = src.m_indexCount;
        m_textures = src.m_textures;
        src.m_vao = src.m_vbo = src.m_ebo = 0;
    }

    /// @brief Move assignment operator
    /// @param src
    /// @return moved Mesh
    Mesh& operator=(Mesh&& src) noexcept {
        this->~Mesh();
        m_vao = src.m_vao;
        m_vbo = src.m_vbo;
        m_ebo = src.m_ebo;
        m_vertexCount = src.m_vertexCount;
        m_indexCount = src.m_indexCount;
        m_textures = src.m_textures;
        src.m_vao = src.m_vbo = src.m_ebo = 0;
        return *this;
    }

    /// @brief delete VAO
    ~Mesh();

    /// @brief Bind VAO to gpu
    void bind();

    /// @brief Get index count of Mesh
    /// @return count
    uint32 indexCount() const { return m_indexCount; }

    /// @brief Get vertex count of Mesh
    /// @return count
    uint32 vertexCount() const { return m_vertexCount; }

    /// @brief Get the indices of Model textures bound to this mesh, used by ModelSystem
    /// @return list of texture indices
    const std::vector<usize>& textures() const { return m_textures; }

    /// @brief Addes a texture index to the list of bound textures
    /// @param i index of texture stored in the Model
    void addTextureIndex(usize i) { m_textures.push_back(i); }

private:
    uint32 m_vao{0};
    uint32 m_vbo{0};
    uint32 m_ebo{0};
    uint32 m_vertexCount{0};
    uint32 m_indexCount{0};
    std::vector<usize> m_textures;
};

} // namespace R3
